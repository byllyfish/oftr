
The OpenFlow protocol defines several type of identifiers, such as Instructions,
Actions, and Properties. Libofp uses a naming convention for all such identifiers.

All identifiers are upper-case, and may contain underscores. The prefix designating the type from the OpenFlow specification is left off.  e.g. just 'OUTPUT', 'GOTO_TABLE', not "OFPIT_OUTPUT", "OFPIT_GOTO_TABLE".

All identifiers are encoded as strings; even when represented by a hexadecimal number.

The EXPERIMENTER identifiers are treated specially. The EXPERIMENTER id will include the experimenter_id after a '.'. An experimenter_id is a 32-bit value that specifies the scope of the remaining experimenter data. e.g. 'EXPERIMENTER.0x00101122'

Unknown identifier values are strings that encode the hexadecimal value, prefixed with '0x'. e.g. '0x00ff'.

Instruction Identifiers
-----------------------

Instruction Identifiers will follow an `instruction` or `instructions` label.

```
GOTO_TABLE
WRITE_METADATA
WRITE_ACTIONS
APPLY_ACTIONS
CLEAR_ACTIONS
METER
EXPERIMENTER.[experimenter_id]
0x[hex_value]
```
An Instruction ID is different from an Instruction itself:

 { "instruction" : "GOTO_TABLE", "table_id" : 3 }
 { "instruction" : "0x0102", "data" : "0011223344" }

The Instruction ID in the first instruction is GOTO_TABLE. The Instruction ID in the second instruction is "0x0102".

Action Identifiers
------------------

Action identifiers will follow an `action` or `actions` label.

OUTPUT
COPY_TTL_OUT
COPY_TTL_IN
SET_MPLS_TTL
DEC_MPLS_TTL
PUSH_VLAN
POP_VLAN
PUSH_MPLS
POP_MPLS
SET_QUEUE
GROUP
SET_NW_TTL
DEC_NW_TTL
SET_FIELD
PUSH_PBB
POP_PBB
EXPERIMENTER.[experimenter_id]
0x[hex_value]

## OpenFlow 1.0 Actions

STRIP_VLAN
ENQUEUE

Property Identifiers
--------------------

Properties are encoded inline whenever possible. This applies to defined properties that can only have a single value. Ommitted properties are set to null, or a default value.

For an example, see the definition of Queue.

Experimenter properties and other properties which may appear multiple times appear in a list under the `properties` label.

EXPERIMENTER.[experimenter_id]

## Table Feature Properies

EXPERIMENTER_MISS.[experimenter_id]


Enumerations
============



Experimenter
============

Experimenter messages, instructions, actions, fields, and properties are identified by an EXPERIMENTER.[experimenter_id] identifier. Many experimenter values expect also a 32-bit exp_type field. This value is *not* explicitly called out anywhere; it is omitted from the YAML input and output. All experimenter-defined data, including `exp_type`, is lumped together.

### Where's `exp_type`?

exp_type is inconsistently applied in different OpenFlow versions. For example, in version 1.4, the experimenter queue property has an exp_type. In version 1.3, it does not. The presence of the exp_type value is therefore treated as experimenter issue, and the simplest thing is just to lump it with the rest of the experimenter data.

Experimenter properties should be rare; as soon as an important experimenter property or type is defined, we can implement it and provide an improved caller API.



