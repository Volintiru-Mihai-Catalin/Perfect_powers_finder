			TEMA1 APD

					Volintiru Mihai Catalin 336CA


	Pentru realizarea acestei teme m-am bazat pe implementarea cu ajutorul
cozilor si a listelor simplu inlantuite (utils.h).
	Cum am gandit tema este in felul urmator:

		- ca sa nu folosesc variabile globale, datele pe care le-am considerat
		de interes am ales sa le pun in structura Th, structura care ma ajuta
		sa dau ca parametru pentru threaduri toate datele necesare.

		- am parsat datele de intrare si dupa aceea, am citit din fisierul dat
		ca argument si am pus fisierele prin care trebuie sa treaca MAP-erii 
		intr-o coada; astfel, unui thread i se atribuie un fisier pe baza 
		operatiei de dequeue din coada (pe care este pusa un mutex astfel incat
		sa evit undefined behaviour) si dupa ce il termina, se uita sa vada daca
		mai este ceva in coada ce poate lua, astfel ca am facut impartirea
		dinamica a fisierelor. Logica principala a acestui lucru se afla in
		functiile "assignFiles" si "func".

		- in continuare, lucrez cu o lista de liste, una pentru fiecare thread
		("set_list"); dupa ce un thread primeste fisierul, acesta se uita pe
		el, si pentru fiecare numar pe care il citeste (mai putin primul),
		ia fiecare putere pe rand (am ales sa fac asta descrescator) si, pe
		baza unei functii care calucleaza niste limite, acesta verifica daca
		numarul citit este putere perfecta sau nu (voi reveni la functia care
		calculeaza limitele mai tarziu). In cazul in care este, aceasta il 
		adauga la lista specifica puterii pentru care a calculat.

		- pentru a astepta ca threadurile de tip MAP sa termine, am implementat
		o bariera, astfel incat threadurile RED sa nu o ia inainte.

		- dupa ce threadurile de tip MAP termina, RED-ucerii iau listele cu
		duplicate si neordonate si le sorteaza. Dupa ce le-a sortat, cauta
		valorile duplicate si le contorizeaza, ca mai apoi sa afiseze in fisier
		numarul lor exact.


	Functia care calculeaza limitele:

		-functia se afla in Limits.h; aceasta functie pleaca de la premisa:

			x^p = nr => x = 2 ^ (log nr / p), unde logaritmul este in baza 2

		- avand in vedere ca exponentul lui 2 are sanse mari sa fie o putere 
		care sa nu fie intreaga, va trebui sa gasesc o limita inferioara si 
		una superioara care sa ma ajute sa gasesc in timp util valoarea exacta
		a lui x, daca acesta exista. Astfel, partea intreaga a puterii o 
		pastrez, iar partea fractionara incerc sa o marginesc.

		- initial, partea fractionara a exponentului se afla intre [0, 1). Avand
		in vedere ca pot sa lucrez cu radicali, o sa aleg o "fractiune" initial
		egala cu 0.5 (nu intamplator, pentru ca sqrt(x) = x^0.5) si incerc sa
		imi dau seama unde se situeaza partea fracionara mai exact, adica 
		[0, 0.5] sau [0.5, 1);

		- sa zicem ca se afla in primul interval, asa ca o sa injumatatesc iar 
		fractiunea pentru a incerca sa gasesc o marginire mai convenabila (din
		nou, nu intamplator 0.25, astfel incat sqrt(sqrt(x)) = x^0.25) intre
		intervalele [0, 0.25] si [0.25, 0.5]. sa zicem ca se afla in al doilea
		interval, astfel ca pana acum, as avea ca:
			[p] + 0.25 <= p <= [p + 1] - 0.5 care e echivalent cu:

			2 ^([p] + 0.25) <= 2^p <= 2^([p + 1] - 0.5)

		- astfel, eu reusesc sa imi scriu puterea fractionara ca suma dintre
		partea ei intreaga si jumatati cat mai mici ale fractiunii initiale 0.5,
		practic 2^p il calculez astfel: ridic efectiv pe 2 la partea intreaga a
		puterii si dupa partea fractionara o scriu ca suma de radicale cu ordin 
		par, ceva in genul acesta: 2^p = 2^[p] * sqrt(2) * sqrt(sqrt(2)) * 
		* sqrt(sqrt(sqrt(2))) * .... si tot asa pentru a gasi mariginile cat 
		mai exacte si pentru a fi usor de calculat, avand in vedere ca pot sa
		lucrez cu radicalul de ordin 2. Normal, am ales ca precizia sa fie de
		50 de iteratii, dar am luat si o marja de eroare a limitelor de +/- 100
		ca sa fiu sigur ca nu sar peste vreo baza care se poate potrivi unui numar
		citit din fisierele respective.

		- eficienta problemei sta undeva in echilibrul dintre PRECISION si 
		ERROR_SCALE care trebuie balansate pentru a face operatiile in timp
		cat mai bun si pentru a le calcula cat mai corect.



