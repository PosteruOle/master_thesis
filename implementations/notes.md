# Prepoznavanje CRC algoritma i zamena optimizovanom verzijom za RISC-V artitekturu u okviru kompajlerske infrastrukture LLVM

Kandidati za naslov:
- Prepoznavanje CRC algoritma i zamena optimizovanom verzijom za RISC-V artitekturu u okviru kompajlerske infrastrukture LLVM
- Prepoznavanje CRC algoritma za potrebe RISC-V artitekture a u okviru kompajlerske infrastrukture LLVM
- Semantička detekcija CRC algoritma za potrebe RISC-V artitekture a u okviru kompajlerske infrastrukture LLVM
- Semantička detekcija algoritma za generisanje kontrolne sume za potrebe RISC-V artitekture a u okviru kompajlerske infrastrukture LLVM

Odabran naslov:
- Automatska detekcija i optimizacija algoritma CRC u okviru kompajlerske infrastrukture LLVM

## LLVM infrastruktura
LLVM je veoma moćan kompilator koji predstavlja ozbiljnog konkurenta GCC kompilatoru.
Svaki kompilator se sastoji od tri dela: frontend-a, middleend-a i backend-a. Kao ulaz kompilatoru se prosleđuje program napisan na višem programskom jeziku, 
a kao izlaz iz kompilatora dobijamo ili objektni fajl ili izvršivu datoteku.
Nakon prolaska kroz prednji deo kompilatora, program prosleđen kompilatoru biva predstavljen među-reprezentacijom (Intermediete Representation iliti IR) koja je arhitekturalno nezavisna.
Implementacije koje će u nastavku rada biti predstaljene su prepoznavanje neoptimizovane forme CRC algoritma sprovodile na IR nivou. 

## CRC algoritam
Algoritam CRC (Cyclic Redundancy Check) predstavlja vrlo koristan algoritam, koji se, baš zbog svoje korisnosti, široko upotrebljava. 
Njegovom primenom se može utvrditi da li su bitovi poruke poslate putem mreže eventualno izmenjeni.


## RISC-V arhitektura

## Implementacija 1
Prva implementacija je u potpunosti sprovedena na IR-nivou. To znači da je program napisan u programskim jezicima C ili C++ preveden prednjim delom LLVM kompilatora u fajl sa .ll ekstenzijom
koji sadrži među-reprezentaciju početnog programa i da je potom isti .ll fajl izmenjen tako da sadrži optimizovanu verziju CRC algoritma koja se potom može proslediti srednjem i zadnjem delu 
kompilatora kako bi se dobio objektni ili izvršivi fajl koji sadrži optimozovanu verziju CRC algoritma. 
Ova implementacija počiva na prepoznavanju jedne varijante CRC algoritma, zatim uklanjanju odnosno brisanju prepoznatih instrukcija neoptimizovane verzije CRC algoritma i na kraju generisanju 
novih IR instrukcija koje pripadaju optimozovanoj verziji algoritma. Time se od jedne verzije .ll fajla dobija novi, takođe ispravan, .ll fajl. 

## Implementacija 2
Druga implementacija počiva na prepoznavanju jedne neoptimizovane varijante CRC algoritma, zatim uklanjanju prepoznatih istrukcija, potom kreiranju poziva intrinzičke funkcije nad istim argumentima
nad kojima bi neoptimizovani CRC algoritam bio sproveden i na kraju pred sam kraj prevođenje programa zamenom intrinzičke funkcije sekvencom mašinskih instrukcija koje pripadaju optimizovanoj varijanti
CRC algoritma.

