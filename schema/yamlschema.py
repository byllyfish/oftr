import yaml
import jsonschema
import os
import sys
from urllib2 import urlopen

"""
yamlschema
==========

Tool for validating the contents of YAML document streams against
a YAML/JSON Schema.


"""

SCHEMA_EXTENSION = '.schema.yml'
SCHEMA_EXAMPLES = 'yamlschema_examples'


def _loadFile(uri):
    """
    Load the YAML document from a file URL.
    """
    
    print uri
    assert uri.startswith('file:')
    uri += SCHEMA_EXTENSION
    return yaml.load(urlopen(uri).read().decode('utf-8'))
    

def _makeValidator(schemaPath):
    """
    Make a JSON schema validator for file `schemaPath`.
    """

    # Get absolute path to schema.
    schemaPath = os.path.abspath(schemaPath)
    
    # Load schema from schemaPath.
    with open(schemaPath) as schemaFile:
        schema = yaml.load(schemaFile)
    
    # Obtain the schema's directory to use as a base url.
    schemaDir = os.path.dirname(schemaPath)
    schemaUrl = 'file://%s/' % schemaDir

    # Make the validator.
    resolver = jsonschema.RefResolver(schemaUrl, schema, handlers=[('file', _loadFile), ('http', _loadFile)])
    validator = jsonschema.Draft4Validator(schema, resolver=resolver)    

    # Check the schema using my RefResolver.
    schemaValidator = jsonschema.Draft4Validator(validator.META_SCHEMA, resolver=resolver)
    schemaValidator.validate(schema)

    return validator


def validateString(sourceString, schemaPath):
    """
    Validate the YAML documents in `sourceString` against the JSON schema in 
    file `schemaPath`.
    """
    
    validator = _makeValidator(schemaPath)

    # Validate all the YAML documents in sourceFile.
    for doc in yaml.load_all(sourceString):
        validator.validate(doc)
    
    return True    


def validateFile(sourcePath, schemaPath):
    """
    Validate the YAML documents in file `sourcePath` against the JSON schema in 
    file `schemaPath`.
    """
    
    validator = _makeValidator(schemaPath)

    # Validate all the YAML documents in sourceFile.
    with open(sourcePath) as sourceFile:
        for doc in yaml.load_all(sourceFile):
            validator.validate(doc)
    
    return True


def checkSchema(schemaPath):
    """
    Check a schema to make sure it's a valid JSON-Schema. If the YAML document
    has a `yamlschema_examples` value, check it using the schema.
    """
    
    validator = _makeValidator(schemaPath)
    
    with open(schemaPath) as schemaFile:
        schema = yaml.load(schemaFile)

    if SCHEMA_EXAMPLES in schema:
        testString = schema[SCHEMA_EXAMPLES]
        for doc in yaml.load_all(testString):
            print doc
            validator.validate(doc)

    return True


if __name__ == '__main__':
    for arg in sys.argv[1:]:
        checkSchema(arg)
