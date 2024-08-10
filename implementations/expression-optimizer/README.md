# Expression optimizer pass

U ovom folderu se nalazi implentacija LLVM prolaza koji ima za cilj da detektuje prošireni oblik 
kvadrata binoma i da detektovani aritmetički izraz zameni drugim koji predstavlja kompaktniju 
odnosno kraću verziju kvadrata binoma.

Uputstvo za kreiranje novih LLVM prolaza možete pronaći na sledećem [linku](https://llvm.org/docs/WritingAnLLVMNewPMPass.html).
