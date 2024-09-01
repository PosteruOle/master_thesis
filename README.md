# Automatska detekcija i optimizacija algoritma CRC u okviru kompajlerske infrastrukture LLVM
Ovaj repozitorijum sadrži implementaciju i tekst master rada na temu "Automatska detekcija i optimizacija algoritma CRC u okviru kompajlerske infrastrukture LLVM". 

Master rad predstavlja poslednju stavku koju je potrebno ispuniti kako bi se uspešno završile master studije. 

Master rad predstavljen u ovom repozitorijumu je urađen za potrebe master studija na Matematičkom fakultetu Univerziteta u Beogradu.

## Apstrakt

U savremenoj industriji razvoja softvera, efikasnost i optimizacija koda predstavljaju ključne aspekte u postizanju visokih performansi računarskih sistema. 
Ovaj master rad istražuje inovativan pristup prevođenju algoritma CRC (eng.Cyclic Redundancy Check) korišćenjem kompilatorske infrastrukture LLVM. 
Algoritam CRC detektuje potencijalne promene u podacima nastalim usled transfera kroz različite medijume (žičane mreže, bežične mreže ili optičke kablove) i ima
široku primenu u digitalnoj komunikaciji, gde se koristi za proveru integriteta podataka. Zbog svoje učestale primene važno je koristiti optimizovane verzije ovog
algoritma.


Cilj ovog rada je unapređenje infrastrukture LLVM u kontekstu prevođenja algoritma CRC, i na taj način ostvarivanje boljih performansi programa koji koriste 
algoritam CRC i LLVM kao svoj kompilator. Osnovna ideja rešenja predstavljenog u radu zasniva se na detekciji neoptimizovane verzije algoritma CRC
na nivou LLVM međureprezentacije i zamenjivanju funkcionalno ekvivalentnom optimizovanom verzijom. Unapređenje je vidljivo na različitim procesorskim arhitekturama, 
specijalno i na procesorskoj arhitekturi RISC-V. Rezultati dobijeni testiranjem predstavljenog rešenja pokazuju značajno poboljšanje performansi algoritma CRC prevedenog LLVM kompilatorom.

## Sadržaj:
U nastavku je naveden sadržaj master rada:
- Uvod 
- Kompilatori i projekat LLVM
  - Osnovne vrste prevodilaca 
  - Faze prevođenja 
  - Bitni delovi kompilatora 
  - Kompilatorske optimizacije 
  - Kompilatorska infrastruktura LLVM 
  - LLVM međureprezentacija 
  - LLVM infrastruktura za testiranje 
- Algoritam CRC i problem njegovog prepoznavanja 
  - Algoritam CRC 
  - Problem prepoznavanja algoritma CRC 
- Procesorske arhitekture i arhitektura RISC-V
  - Procesorske arhitekture RISC i CISC
  - Arhitektura RISC-V 
- Implementacija i evaluacija rešenja 
  - Implementacija na IR nivou
  - Implementacija pomoću intrinzičkih funkcija
  - Regresioni testovi 
  - Funkcionalna ispravnost implementacije
  - Rezultati evaluacije efikasnosti optimizacije
- Zaključak

## Uvod
Rad se sastoji od šest poglavlja. U poglavlju 2 se govori o procesu prevođenja programa, 
metodologijama konstrukcije kompilatora i o kompilatorskoj infrastrukturi LLVM. 
Predstavljena je istorija samog projekta, arhitektura projekta, delovi kompilatora i međureprezentacija koju LLVM koristi. 
Dodatno, opisana je i infrastruktura za testiranje. Poglavlje 3 je posvećeno algoritmu CRC i problemu
njegovog prepoznavanja. Predstavljen je koncept detektovanja grešaka u podacima, zajedno sa trenutnim načinima korekcije
i detekcije grešaka. Poglavlje 4 je posvećeno procesorskim arhitekturama, sa akcentom na procesorsku arhitekturu
RISC-V jer je baš za nju predložena jedna optimizacija u okviru ovog rada. U poglavlju 5 su predstavljena dva rešenja 
problema detektovanja algoritma CRC i njegovog optimizovanja. Na početku poglavlja su opisani ideja i postupak zajednički
za oba rešenja, a u nastavku su redom nalaze detalji implementacije svakog od predloženih rešenja. 
Poslednja sekcija u poglavlju 5 opisuje rezultate eksperimentalne evaluacije predloženih rešenja. 
U poglavlju 6 je sumirano sve što je predstavljeno u radu i ostavljene su smernice i sugestije čitaocima rada
za dalje istraživanje na temu unapređenja LLVM i drugih kompilatora.

## Problem prepoznavanja algoritma CRC
Problem koji se u ovom radu rešava jeste prepoznavanje (detektovanje) algoritma CRC i njegovo optimizovanje. 
Pod prepoznavanjem se podrazumeva pronalaženje njegove implementacije u izvornom kodu nekog programa. 
Da bi prepoznavanje algoritma CRC bilo izvodiljivo potrebno je da njegova implementacija
bude unapred poznata. Prepoznavanje se može izvršiti prolaskom kroz kôd programa (krećući se od prve 
ka poslednjoj instrukciji ili u obrnutom smeru). Ovim postupkom se vrši upoređivanje redosleda instrukcija 
programa sa redosledom instrukcija algoritma CRC. Takođe se proverava da li svaka od instrukcija programa
sadrži argumente istog tipa i iste vrednosti kao odgovarajuća instrukcija u algoritmu CRC. 
Različit redosled nezavisnih instrukcija programa ne bi trebalo da utiče na rezultat prepoznavanja. 
Korišćenje različitih, funkcionalno ekvivalentnih instrukcija u programu takođe ne bi trebalo da utiče na rezultat prepoznavanja.
Na kraju ovakvog postupka, ukoliko ni u jednoj od spomenutih provera nije bilo
razlike između očekivanih i dobijenih rezultata, može se zaključiti da je algoritam
prepoznat (odnosno detektovan). Tek tada se mogu pokrenuti akcije za njegovo
optimizovanje. 


Problem sa prepoznavanjem algoritma CRC jeste taj što postoji veliki broj njegovih implementacija, 
pa je za svaku od njih potrebno napraviti poseban šablon za prepoznavanje (eng. pattern matcher). 
Pod šablonom za prepoznavanje se podrazumevaju sve provere koje se nad jednim programom vrše (prolazak kroz njegove
instrukcije, provera njihovog redosleda, vrednosti njihovih operanada i druge) kako
bi se ustanovilo da li on u sebi sadrži implementaciju nekog algoritma. Razvijanje
svakog od šablona zahteva dosta vremena i truda kako bi se napravio šablon sposoban da prepozna 
što veći broj modifikacija iste verzije algoritma. Sa druge strane,
prepoznavanje jednostavnijih algoritama ili aritmetičkih izraza (proširenog oblika
kvadrata binoma, kuba binoma, izraza za računanje rešenja kvadratne jednačine)
je daleko jednostavnije i zahteva pokrivanje znatno manjeg broja slučajeva1.


Kako se usled porasta broja informacija koje se prosleđuju putem interneta, povećava upotreba algoritma CRC
i drugih metoda za proveru integriteta podataka tako raste potreba da svaka od korišćenih metoda bude što efikasnija. 
Jedan od načina da se obezbedi efikasnost ovih algoritama jeste upravo uvođenje dodatnih provera u prevodiocima 
kojima bi se ovi algoritmi detektovali i potom optimizovali.


U okviru repozitorijuma nalazi se i implementacija LLVM optimizacionog prolaza pod nazivom expression-optimizer. Svrha tog optimizacionog
prolaza jeste detektovanje proširenog oblika kvadrata binoma (a2 + 2ab + b2) i zamena istog odgovarajućim skrećenim zapisom ((a + b)2)


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

## Zaključak:
Cilj ovog rada je da predstavi jedan pravac unapređenja kompilatorske infrastrukture LLVM
i da precizno opiše način na koji je takva unapređenja moguće integrisati u projekat LLVM. 
Kao ilustracija ove vrste unapređenja odabran je problem detekcije i optimizacije algoritma CRC
zbog važnosti i sve učestalije primene ovog algoritma.


Glavni predmet rada je uvođenje novog optimizacionog prolaza u okviru projekta LLVM koji
na nivou LLVM međureprezentacije detektuje određenu sekvencu IR instrukcija i zamenjuje je drugom. 
U radu su predstavljene dve mogućnosti, prva koja sekvencu IR instrukcija zamenjuje drugom sekvencom IR instrukcija i
druga koja sekvencu IR instrukcija zamenjuje pozivom intrinzičke funkcije. Oba rešenja sadržana su u kreiranom optimizacionom prolazu. 
Ideja predloženih rešenja može biti iskorišćena za prepoznavanje i optimizovanje drugih verzija algoritma
CRC kao i potpuno drugih algoritama i aritmetičkih izraza.


Doprinos ove teze jeste predstavljanje metoda za prepoznavanje nekog algoritma korišćenjem kompilatora LLVM. 
Svrha prepoznavanja i optimizovanja nekog algoritma jeste da se poveća efikasnost programa dobijenog na samom kraju
prevođenja. Efikasnost se odnosi na smanjenje vremena izvršavanja i smanjenje memorijskog zauzeća programa. 
Pokretanjem implementacije na IR nivou postiže se smanjenje vremena izvršavanja neoptimizovanog algoritma CRC za čak 37%.
Postoji nekoliko pravaca u kojima bi predložena rešenja mogla biti poboljšana. Trenutne implementacije funkcionišu isključivo 
za pristup odabiru instrukcija ostvarenom kroz SelectionDAG. U budućnosti bi trebalo podržati pristup selekcije
instrukcija GlobalIsel koji se sve više koristi.


Detektovanje i optimizovanje algoritma CRC bi moglo biti omogućeno za veliki broj drugih verzija algoritma primenom pristupa
predstavljenog u ovom radu. Postupak prepoznavanja bi mogao biti poboljšan tako da prepoznaje još neke verzije neoptimizovanog algoritma CRC.
Predlog implementacije pomoću intrinzičkih funkcija bi mogao biti izmenjen tako da bude podržan i na drugim arhitekturama.
Nadam se da će ovaj rad čitaocima pomoći da se bolje upoznaju sa projektom LLVM, algoritmom CRC i uopšte postupkom optimizovanja nekog algoritma. 
Takođe, nadam da će rad dati motivaciju i smernice za dalje unapređivanje LLVM i drugih kompilatora kako bi se i neki drugi algoritmi, 
aritmetički izrazi ili često korišćene programske konstrukcije prepoznavali, optimizovali i kasnije efikasnije izvršavali.

## Autori:
Mentor: dr Milena Vujošević Janičić

Student: Petar Tešić 1064/2022 5I

Master rad je urađen u saradnji sa kompanijom SYRMIA.
