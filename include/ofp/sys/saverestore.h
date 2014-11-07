

#ifndef OFP_SYS_SAVERESTORE_H_
#define OFP_SYS_SAVERESTORE_H_

OFP_BEGIN_IGNORE_PADDING

// RAII Utility class to prevent modification while iterating. (Not thread-safe)

template <class Type>
class SaveRestore {
 public:
  SaveRestore(Type &ref, Type val) : ref_{ref}, val_{ref} { ref_ = val; }

  ~SaveRestore() { ref_ = val_; }

 private:
  Type &ref_;
  Type val_;
};

OFP_END_IGNORE_PADDING

#endif  // OFP_SYS_SAVERESTORE_H_
