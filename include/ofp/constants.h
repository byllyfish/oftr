#ifndef OFP_CONSTANTS_H
#define OFP_CONSTANTS_H

namespace ofp { // <namespace ofp> 

enum {
    OFPVID_PRESENT = 0x1000,
    OFPVID_NONE = 0x0000
};

enum {
    OFPMT_STANDARD = 0,
    OFPMT_OXM = 1
};

/* Values for 'type' in ofp_error_message.  These values are immutable: they
 * will not change in future versions of the protocol (although new values may
 * be added). */

enum {
    OFPET_HELLO_FAILED = 0,           /* Hello protocol failed. */
    OFPET_BAD_REQUEST = 1,            /* Request was not understood. */
    OFPET_BAD_ACTION = 2,             /* Error in action description. */
    OFPET_BAD_INSTRUCTION = 3,        /* Error in instruction list. */
    OFPET_BAD_MATCH = 4,              /* Error in match. */
    OFPET_FLOW_MOD_FAILED = 5,        /* Problem modifying flow entry. */
    OFPET_GROUP_MOD_FAILED = 6,       /* Problem modifying group entry. */
    OFPET_PORT_MOD_FAILED = 7,        /* Port mod request failed. */
    OFPET_TABLE_MOD_FAILED = 8,       /* Table mod request failed. */
    OFPET_QUEUE_OP_FAILED = 9,        /* Queue operation failed. */
    OFPET_SWITCH_CONFIG_FAILED = 10,  /* Switch config request failed. */
    OFPET_ROLE_REQUEST_FAILED = 11,   /* Controller Role request failed. */
    OFPET_METER_MOD_FAILED = 12,      /* Error in meter. */
    OFPET_TABLE_FEATURES_FAILED = 13, /* Setting table features failed. */
    OFPET_EXPERIMENTER = 0xffff       /* Experimenter error messages. */
};

/* ofp_error_msg 'code' values for OFPET_HELLO_FAILED.  'data' contains an
 * ASCII text string that may give failure details. */
enum ofp_hello_failed_code {
    OFPHFC_INCOMPATIBLE = 0, /* No compatible version. */
    OFPHFC_EPERM = 1,        /* Permissions error. */
};


/* Hello elements types.
 */
enum ofp_hello_elem_type {
    OFPHET_VERSIONBITMAP          = 1,  /* Bitmap of version supported. */
};

} // </namespace ofp>

#endif // OFP_CONSTANTS_H
