# Prepoznavanje CRC algoritma i zamena optimizovanom verzijom za potrebe RISCV arhitekture u okviru kompajlerske infrastrukture LLVM
Ovaj repozitorijum će sadržati implementaciju i objašnjene master rada na temu "Prepoznavanje CRC algoritma i zamena optimizovanom verzijom za potrebe RISCV arhitekture u okviru kompajlerske infrastrukture LLVM". 

Master rad predstavlja poslednju stavku koju je potrebno ispuniti kako bi se uspešno završile master studije. 

Master rad koji će u ovom repozitorijumu biti predstavljen je urađen za potrebe master studija na Matematičkom fakultetu Univerziteta u Beogradu.

## Sadržaj:
U nastavku su navedene glavne stavke koje je u radu potrebno objasniti. Lista stavki će se potencijalno menjati:
- Postavka problema
- Faze prevođenja programa
- Delovi kompilatora (frontend, middleend, backend)
- Kompajlerska infrastruktura LLVM
- Arhitektura RISCV
- Algoritam CRC (Cyclic Redundancy Check)

## Postavka problema
Problem koji u ovom radu rešavamo je, kao što i sam naslov kaže, prepoznavanje neoptimizovane verzije CRC algoritma, a potom, po uspešnom prepoznavanju i zamena prepoznate (opet neoptimizovane) verzije optimizovanom verzijom. <br>
Ukoliko na kratko razmislimo kako bismo to bilo koji algoritam mogli da prepoznamo dolazimo do zaključka da je to jedino moguće uraditi sprovođenjem čitavog algoritma i upoređivanjem da li se u svakom od koraka izvršava ona naredba ili instrukcija koju očekujemo i proveravanjem da li svaka od promenljivih koje figurišu u algoritmu u svakom trenutku ima vrednost koju mi očekujemo da će imati. <br> 
Na kraju ovakvog postupka, ukoliko ni u jednoj od spomenutih provera nije bilo razlike između očekivanih i dobijenih rezultata, možemo slobodno zaključiti da smo uspešno prepoznali algoritam od našeg interesa.

Neka je u nastavku naveden sadržaj fajla **crc_unoptimized_version.c** (fajla koji sadrži neoptimizovanu verziju CRC algoritma):
```
#include <stdio.h>
#include <stdlib.h>

unsigned short crcu8(unsigned char data, unsigned short crc) {
    unsigned char i = 0, x16 = 0, carry = 0;
    for (i = 0; i < 8; i++) {
      x16 = (unsigned char)((data & 1) ^ ((unsigned char)crc & 1));
      data >>= 1;
      if (x16 == 1) {
        crc ^= 0x4002;
        carry = 1;
      } else
       carry = 0;
      crc >>= 1;
     if (carry)
      crc |= 0x8000;
     else
      crc &= 0x7fff;
     }
    return crc;
}

int main(){

  unsigned char data=144;
  unsigned short crc=12;
  unsigned short report=crcu8(data, crc);
  printf("report = %u\n", report);

  return 0; 
}
```

Neka je u nastavku naveden sadržaj fajla **crc_optimized_version.c** (fajla koji sadrži optimizovanu verziju CRC algoritma):
```
#include <stdio.h>
#include <stdlib.h>

unsigned short crcu8_optimized(unsigned char data, unsigned short _crc)  {
    unsigned char i = 0, x16 = 0, carry = 0;
    long crc = _crc;
    crc ^= data;
    for (i = 0; i < 8; i++) {
      x16 = (unsigned char)crc & 1;
      data >>= 1;
      crc >>= 1;
      crc ^= (x16 & 1) ? 0xa001 : 0; // Conditional XOR
    }
    return crc;
}


int main(){

  unsigned char data=144;
  unsigned short crc=12;
  unsigned short report=crcu8_optimized(data, crc);
  printf("report = %u\n", report);
   
  return 0; 
}
```
Kao što verovatno već pretpostavljate, želimo da prepoznamo verziju CRC algoritma iz fajla **crc_unoptimized_version.c** i da je potom zamenimo verzijom istog algoritma iz fajla **crc_optimized_version.c**. <br>
Međutim, ne želimo da menjamo sadržaj **crc_unoptimized_version.c** fajla, već želimo da nad njim započnemo proces kompilacije tokom koga će kompilator koji budemo koristili (LLVM u našem slučaju) ustanoviti da se radi o neoptimozovanoj implementaciji i potom umesto nje iskoristiti optimizovanu. <br>
**Kako ćemo tako nešto postići?** <br>
Da bismo odgovorili na ovo pitanje potrebno je da se prvo upoznamo sa različitim nivoima apstrakcije i reprezentacije koda koji se prevodi LLVM kompilatorom i uopšte načinom prevođenje programa korišćenjem LLVM kopilatora.  

# Faze prevođenja programa
Proces prevodjenja programa je ključni korak u transformaciji izvornog koda napisanog na programskom jeziku visokog nivoa u oblik koji je računaru razumljiv. 
Postoje dva osnovna pristupa prevođenju programa i to su kompilacija i interpretacija.

Kompilacija podrazumeva prevođenje celog izvornog koda programa u niz instrukcija koje računar može direktno da izvrši. 
Ovaj proces se obično sastoji iz nekoliko faza. Prva faza je faza leksičke analizi, u kojoj 
se izvorni kod razlaže na niz tokena kao što su ključne reči, operatori i identifikatori. Zatim sledi faza sintaksne analize, gde se proverava ispravnost sintakse izvornog koda prema gramatici jezika. Nakon toga dolazi faza semantičke analize, gde se proveravaju semantička pravila jezika i proizvode se apstraktna sintaksna stabla. U sledećoj fazi se generiše međukod koji je specifičan za ciljnu arhitekturu. Na kraju, međukod se prevodi u mašinski kod odgovarajuće ciljne arhitekture.

Interpretacija, s druge strane, podrazumeva izvršavanje izvornog koda redom, liniju po liniju, koristeći interpreter. Tokom interpretacije, izvorni kod se ne prevodi unapred u mašinski kod, već se svaka instrukcija izvršava u trenutku kada se naiđe na nju. Ovo omogućava dinamičko izvršavanje koda, ali često može rezultirati sporijim izvršavanjem u poređenju sa kompilacijom.

Iako kompilacija i interpretacija imaju različite faze i pristupe, oba procesa imaju isti cilj: da prevedu izvorni kod programa u oblik koji računar može razumeti i izvršiti. Izbor između kompilacije i interpretacije zavisi od specifičnih zahteva i karakteristika programa, kao i preferencija programera.


## Kompajlerska infrastruktura LLVM
Projekat LLVM započet je 2000. godine na Univerzitetu Ilinois od strane Krisa
Latnera. Cilj projekta je bio proučavanje tehnika kompiliranja u SSA obliku (eng. Static Single Assign-
ment) koje podržavaju statičku i dinamičku kompilaciju proizvoljnih programskih
jezika. Naziv LLVM je predstavljao akronim za „virtuelna mašina niskog nivoa”
(eng. Low Level Virtual Machine). Međutim, isti akronim se više ne koristi, ali je ime projekta
ostalo nepromenjeno. Danas, projekat sadrži veliki broj biblioteka i alata koji se
koriste kako u komercijalne svrhe tako i u svrhe razvoja projekata otvorenog koda. Svaki deo projekta je dizajniran 
kao biblioteka tako da se može ponovo upotrebiti za implementiranje drugih
alata. Celokupan izvorni kôd je javno dostupan na servisu GitHub i oko njega je formirana velika zajednica ljudi koji 
rade na različitim delovima LLVM i svakodnevno ga unapređuju. Veliki broj kompanija koristi svoje verzije kompilatora 
LLVM bilo u celosti bilo samo neke njegove delove (prednji, srednji ili zadnji deo kimpilatora) za podršku neke 
arhitekture ili kao osnovu za novi programski jezik.

### Proces prevođenja programa u okviru kompajlera LLVM
Ponovimo još jednom, kao i većina drugih kompilatora, i LLVM se sastoji od tri bitna dela: prednjeg, srednjeg i zadnjeg dela.
Za svaki deo kompilatora je odgovoran različiti alat. 
Za prednji deo LLVM-a odgovoran je alat Clang, za srednji deo LLVM-a odgovaran je alat opt, a za poslednji odgovoran je alat llc.
Prevođenje programa napisanog u programskom jeziku C ili C++ kompajlerom LLVM počinje tako što se program prvo prosledi Clang-u, koji od njega formira fajl sa ekstenzijom .ll
u kome se nalazi među-reprezentacija (intermediate representation) početnog programa. Taj fajl se potom prosleđuje alatu opt koji nad istim vrši veliki broj analiza i optimizacija
kako bi od njega kreirao optimizovanu među-reprezentaciju našeg početnog programa. Izlaz opt alata se na kraju prosleđuje alatu llc koji nad njim vrši određene trasnformacije kako bi 
se na samom kraju LLVM-ovog pipeline-a dobio ili asemblerski kod ili objektni fajl.

## Algoritam CRC (Cyclic Redundancy Check)
CRC (Cyclic Redundancy Check) je algoritam koji se često koristi za proveru integriteta podataka u digitalnim komunikacijama. Njegova bitnost proizilazi iz njegove sposobnosti otkrivanja grešaka koje se mogu desiti prilikom prenosa podataka putem različitih medija, kao što su žičane veze, bežične mreže ili optički kablovi.
Аlgoritam se često koristi u računarskim mrežama, komunikacionim sistemima, kao i u memorijskim uređajima.
Princip rada CRC algoritma se zasniva na generisanju kratkog kontrolnog broja koji se dodaje na kraj porukCRC (Cyclic Redundancy Check) je algoritam koji se često koristi za proveru integriteta podataka u digitalnim komunikacijama. Njegova bitnost proizilazi iz njegove sposobnosti otkrivanja grešaka koje se mogu desiti prilikom prenosa podataka putem različitih medija, kao što su žičane veze, bežične mreže ili optički kablovi.

Ovaj algoritam se često koristi u računarskim mrežama, komunikacionim sistemima, kao i u memorijskim uređajima. On omogućava efikasno otkrivanje grešaka koje se javljaju usled slučajnih promena u podacima tokom prenosa.

Princip rada CRC algoritma se zasniva na generisanju kratkog kontrolnog broja koji se dodaje na kraj poruke pre slanja. Ovaj kontrolni broj se računa na osnovu samih podataka poruke, i njegova dužina zavisi od izabrane CRC funkcije.e pre slanja. Ovaj kontrolni broj se računa na osnovu samih podataka poruke, i njegova dužina zavisi od izabrane CRC funkcije.

## Zaključak:

## Autori:
Mentor: dr Milena Vujošević Janičić

Student: Petar Tešić 1064/2022 5I

Master rad je urađen u saradnji sa kompanijom SYRMIA.
