# Prepoznavanje CRC algoritma i zamena optimizovanom verzijom za RISC-V artitekturu u okviru kompajlerske infrastrukture LLVM


## LLVM infrastruktura
LLVM je veoma moćan kompilator koji predstavlja ozbiljnog konkurenta GCC kompilatoru.
Svaki kompilator se sastoji od tri dela: frontend-a, middleend-a i backend-a. Kao ulaz kompilatoru se prosleđuje program napisan na višem programskom jeziku, 
a kao izlaz iz kompilatora dobijamo ili objektni fajl ili izvršivu datoteku.
Nakon prolaska kroz prednji deo kompilatora, program prosleđen kompilatoru biva predstavljen među-reprezentacijom (Intermediete Representation ili it IR) koja je arhitekturalno nezavisna.
Implementacije koje će u nastavku rada biti predstaljene su prepoznavanje neoptimizovane forme CRC algoritma sprovodile na IR nivou. 

## CRC algoritam
Algoritam CRC (Cyclic Redundancy Check) predstavlja vrlo koristan algoritam, koji se, baš zbog svoje korisnosti, široko upotrebljava. 
Njegovom primenom se može utvrditi da li su bitovi poruke poslate putem mreže eventualno izmenjeni.


## RISC-V arhitektura

## Implementacija 1


## Implementacija 2

