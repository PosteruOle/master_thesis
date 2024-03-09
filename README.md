# Prepoznavanje CRC algoritma i zamena optimizovanom verzijom za potrebe RISCV arhitekture u okviru kompajlerske infrastrukture LLVM
Ovaj repozitorijum će sadržati implementaciju i objašnjene master rada na temu "Prepoznavanje CRC algoritma i zamena optimizovanom verzijom za potrebe RISCV arhitekture u okviru kompajlerske infrastrukture LLVM". 

Master rad predstavlja poslednju stavku koju je potrebno ispuniti kako bi se uspešno završile master studije. 

Master rad koji će u ovom repozitorijumu biti predstavljen je urađen za potrebe master studija na Matematičkom fakultetu Univerziteta u Beogradu.

## Sadržaj:
U nastavku su navedene glavne stavke koje je u radu potrebno objasniti. Lista stavki će se potencijalno menjati:
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


## Kompajlerska infrastruktura LLVM
Projekat LLVM započet je 2000. godine na Univerzitetu Ilinois od strane Krisa
Latnera. Projektu se ubrzo pridružio i njegov mentor Vikram Adve. Cilj projekta
je bio proučavanje tehnika kompiliranja u SSA obliku (eng. Static Single Assign-
ment) koje podržavaju statičku i dinamičku kompilaciju proizvoljnih programskih
jezika. Inicijalno, naziv LLVM je bio akronim za „virtuelna mašina niskog nivoa”
(eng. Low Level Virtual Machine). Akronim se više ne koristi, ali je ime projekta
ostalo nepromenjeno. Danas, projekat sadrži veliki broj biblioteka i alata koji se
koriste u komercijalne svrhe i u projektima otvorenog koda. Svaki deo projekta je dizajniran 
kao biblioteka tako da se može ponovo upotrebiti za implementiranje drugih
alata. Celokupan izvorni kôd je javno dostupan na servisu GitHub. Velika zajednica se formirala što je znatno doprinelo
njegovoj popularnosti. Mnoge firme koriste svoje verzije kompilatora LLVM bilo za
podršku neke arhitekture ili kao osnovu za novi programski jezik.

## Algoritam CRC (Cyclic Redundancy Check)

## Zaključak:

## Autori:
Mentor: dr Milena Vujošević Janičić

Student: Petar Tešić 1064/2022

Master rad je urađen u saradnji sa kompanijom SYRMIA.
