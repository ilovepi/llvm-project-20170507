add_llvm_library(LLVMSyringe
  Syringe.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Syringe

  DEPENDS
  intrinsics_gen
  )
