import yaml
import subprocess
import copy
import sys


OFPX = '../ofpx'
MAX_VERSION = 4
STR15 = '.' * 15
STR31 = '.' * 31
STR255 = '.' * 255

def _getType(elem):
    if elem == None:
        return 'null'
    elif isinstance(elem, dict):
        return 'object'
    elif isinstance(elem, list):
        return 'array'
    elif elem == 0xff or elem == 0x0f:
        return 'uint8'
    elif elem == 0xffff:
        return 'uint16'
    elif elem == 0xffffffff:
        return 'uint32'
    elif elem == 0xffffffffffffffff:
        return 'uint64'
    elif isinstance(elem, str):
        s = elem.lower()
        if s == 'ff:ff:ff:ff:ff:ff':
            return 'macaddress'
        elif s == 'ff:ff:ff:ff:ff:ff:ff:ff' or s == 'ffff-ffff-ffff-ffff':
            return 'datapath'
        elif s == STR15:
            return 'str15'
        elif s == STR31:
            return 'str31'
        elif s == STR255:
            return 'str255'
        elif s == 'ffffffff':
            return 'binary'
        elif s == '255.255.255.255':
            return 'ipv4address'
        else:
            return elem
    else:
        return str(elem)

_syntaxModified = {
    'null': None,
    'uint8': 0x0e,
    'uint16':0xeeee,
    'uint32': 0xeeeeeeee,
    'uint64': 0xeeeeeeeeeeeeeeee,
    'macaddress': 'ee:ee:ee:ee:ee:ee',
    'datapath': 'ee:ee:ee:ee:ee:ee:ee:ee',
    'binary' : 'eeeeeeee',
    'str15' : 'e.e.e.e.e.e.e.e',
    'ipv4address' : '14.14.14.14'
}

_syntaxModifiedValues = [str(v) for v in _syntaxModified.values()] + ['238', 'eeee-eeee-eeee-eeee']

def _modifySyntax(elem):
    syntax = _getType(elem)
    return _syntaxModified.get(syntax, elem)

def _isModified(val, fieldVal):
    return val != fieldVal and val.lower() in _syntaxModifiedValues

def _increment(elem):
    if elem == 0x0f:            # alternate case for uint8
        return 256
    if isinstance(elem, int):
        return elem + 1
    if isinstance(elem, str) and elem and elem[-1] == '.':
        return elem + '.'
    return None

def _annotateDict(elem, keypath, result):
    for key,value in elem.items():
        _annotate(value, '%s.%s' % (keypath, key), result)

def _annotateList(elem, keypath, result):
    for key,value in enumerate(elem):
        _annotate(value, '%s.%d' % (keypath, key), result)

def _annotateElem(elem, keypath, result):
    entry = OFPEntry(keypath, _getType(elem))
    result.append(entry)
    
def _annotate(elem, keypath, result):
    _annotateElem(elem, keypath, result)
    if isinstance(elem, dict):
        _annotateDict(elem, keypath, result)
    elif isinstance(elem, list):
        _annotateList(elem, keypath, result)
   

class OFPEntry(object):
    def __init__(self, keypath, type):
        self.keypath = keypath
        self.type = type
        
    def __repr__(self):
        return str(self.__dict__)

    def __str__(self):
        val = self.type
        if val == '':
            val = "''"
        return '%s=%s' % (self.keypath, val)

    def omit(self, doc):
        # Remove this keypath from the specified document.
        container, key = self.find(doc)
        del container[key]

    def modify(self, doc):
        # Edit this keypath in the specified document.
        container, key = self.find(doc)
        container[key] = _modifySyntax(container[key])
    
    def increment(self, doc):
        # Increment this keypath in the specified document.
        container, key = self.find(doc)
        container[key] = _increment(container[key])
        return container[key] is not None

    def find(self, doc):
        # Find this keypath in the specified document. Return the tuple
        # (container-ref, key).
        next = doc
        keys = self.keypath.split('.')
        for key in keys[:-1]:
            if isinstance(next, list): key = int(key)
            next = next[key]
        key = keys[-1]
        if isinstance(next, list): key = int(key)
        return (next, key)
        
    def default(self):
        if not hasattr(self, 'omittedConsequences'):
            return ''
        oc = getattr(self, 'omittedConsequences')
        result = [str(o) for o in oc]
        return ','.join(result)

    def modified(self):
        if not hasattr(self, 'modifyConsequences'):
            return ''
        return getattr(self, 'modifyConsequences')
        
    def effect(self, consequences):
        #return str(consequences)
        for fld in consequences:
            if self.keypath == fld.keypath:
                return str(fld)
        return ''

def _keypath(obj): return obj.keypath
    
class OFPDocument(object):
    def __init__(self, doc):
        self.doc = doc
        self.fields = []
        _annotate(doc['msg'], 'msg', self.fields)
        # sort fields by keypath.
        #self.fields.sort(key=_keypath)
        
    def __repr__(self):
        return self.type() + str(self.fields)
    
    def type(self):
        # Return type of message. If it's a multipart message, include the 
        # subtype.
        type = self.doc['type']
        if type == 'OFPT_MULTIPART_REQUEST' or type == 'OFPT_MULTIPART_REPLY':
            type = '%s.%s' % (type, self.doc['msg']['type'])
        type = '%s.v%d' % (type, self.doc['version'])
        return type

    def check(self):
        # Test document itself.
        print >> sys.stderr, '  annotate.py: Checking %s' % self.type()
        self.consequences = self.compare(self.roundtrip())
        self.filter(self.consequences)
        # For each field, try roundtripping the document without it.
        for omitField in self.fields:
            result = self.roundtrip(omitField)
            if not result:
                omitField.required = True
            else:
                omitField.required = False
                omitField.omittedConsequences = self.compare(result)
                self.filter(omitField.omittedConsequences)
        # For each field, try roundtripping the document with it modified.
        for modifyField in self.fields:
            result = self.roundtrip(modifyField=modifyField)
            if not result:
                modifyField.modifyConsequences = 'ERROR'
                continue
            # Check value of modifyField in result to see if it changed.
            for fld in result.fields:
                if fld.keypath == modifyField.keypath and _isModified(fld.type, modifyField.type):
                    modifyField.modifyConsequences = True
                    break
        # For each field, try roundtripping the document with it incremented.
        # This should fail; all fields are at their max.
        for modifyField in self.fields:
            result = self.roundtrip(incrementField=modifyField)
            if result:
                print >> sys.stderr, 'Unexpected result:\n' + str(result)
                print >> sys.stderr, modifyField
            assert not result

    def roundtrip(self, omitField=None, modifyField=None, incrementField=None):
        doc = roundtrip(self.doc, omitField, modifyField, incrementField)
        if doc:
            return OFPDocument(doc)
        return None
    
    def compare(self, doc):
        if not doc:
            return []
        # Return list of fields that are different in 'doc'
        lhs = sorted(self.fields, key=_keypath)
        rhs = sorted(doc.fields,key=_keypath)
        # Compare two sorted lists.
        i = 0
        j = 0
        result = []
        while i < len(lhs) and j < len(rhs):
            lf = lhs[i]
            rf = rhs[j]
            if _keypath(lf) < _keypath(rf):
                i += 1
                result.append(lf)
            elif _keypath(lf) > _keypath(rf):
                j += 1
                result.append(rf)
            else:
                assert _keypath(lf) == _keypath(rf)
                if lf.type != rf.type:
                    result.append(rf)
                i += 1
                j += 1
        return result
    
    def filter(self, fields):
        # If first item in list is type null, remove any other fields that
        # share the same keypath prefix.
        if fields:
            if fields[0].type == 'null':
                prefix = fields[0].keypath + '.'
                for i in range(len(fields)-1, -1, -1):
                    if fields[i].keypath.startswith(prefix):
                        del fields[i]

    def toYaml(self):
        return yaml.dump(self.doc)

    def output(self):
        type = self.type()
        version = self.doc['version']
        for field in self.fields:
            row =[version, type, field.keypath, field.type, field.required, field.default(), field.effect(self.consequences), field.modified()]
            print '\t'.join(str(o) for o in row)

# Spawn a subprocess to execute the given arguments; pass it the given input.
# Return tuple (returnCode, stdout) of process.

def spawn(args, input):
    proc = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, stderr = proc.communicate(input)
    assert proc.returncode >= 0 and "ofpx terminated with signal."
    #if proc.returncode:
    #    print >> sys.stderr, 'Output:\n' + stderr
    return (proc.returncode, output)

def roundtrip(doc, omitField=None, modifyField=None, incrementField=None):
    # if omitField is set, make a deep-copy of doc, and remove the specified
    # field.
    if omitField:
        doc = copy.deepcopy(doc)
        omitField.omit(doc)
    if modifyField:
        doc = copy.deepcopy(doc)
        modifyField.modify(doc)
    if incrementField:
        doc = copy.deepcopy(doc)
        if not incrementField.increment(doc):
            return None
    input = yaml.dump(doc)
    #print >> sys.stderr, 'Input:\n' + input
    exit, output = spawn([OFPX, 'encode', '-R'], input)
    if not exit:
        return yaml.load(output)
    return None

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print 'Usage: annotate.py <input-file>'
        sys.exit(1)

    inputFile = sys.argv[1]
    header=['version', 'type', 'keypath', 'syntax', 'required', 'default', 'missing', 'modify']
    print '\t'.join(header)
    for version in range(1,MAX_VERSION+1):
        with open(inputFile) as input:
            docs = yaml.load_all(input)
            for doc in docs:
                if doc:
                    doc['version'] = version
                    d = OFPDocument(doc)
                    d.check()
                    d.output()


