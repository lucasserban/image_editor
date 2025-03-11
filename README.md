# image_editor

1. La inceputul codului am initializat valori de care am nevoie in tot programul, matricile, 
valorile absolute ale coordonatelor de selectie, structura care retine headerul fisierelor, etc.

2. La comanda LOAD verific daca exista vreun fisier deja incarcat, daca exista printez mesajul necesar 
si inlocuiesc pointerul fisierului, dupa ce am aflat ca fisierul exista, cu functia image_info
preiau informatiile headerului acelui fisier in structura creata si dupa aceea verific daca fisierul este
binar sau text. In cazul fisierului binar dupa ce e citit headerul citesc byte cy byte matricea si o bag in matricea imagine
si in cazul celor text citesc numar cu numar. Dupa ce am imaginea in matrice ii fac o dublura pentru a ma ajuta
in comenziile ce urmeaza. Pentru ca la LOAD se face automat si SELECT ALL, setez variabilele de selectie a coordonatelor matricii
corespunzator. Pentru matricile de tip P3 si P6 aloc dinamic latimea matricii triplu deoarece sunt tripleti de valori.

3. La comanda SELECT ALL setez variabilele de selectie la 0 0 si respectiv latimea imaginii si inaltimea acesteia.

4. La comanda SELECT, cu ajutorul functii atoi preiau coordonatele selectiei din comanda si verific daca e posibil ca selectia sa 
aiba acele valori, daca este atunci salvez acele valori in variabilele de selectie.

5. La comanda CROP, eliberez memoria imaginii , ii realoc memorie si cu ajutorul matricii auxiliare si a variabilelor de selectie 
"decupez" imaginea, iar dupa aceasta, eliberez si matricea auxiliara , ii realoc memorie si copiez elementele din imagine in ea
astfel sa am si dupa crop o copie a imaginii.

6. La comanda SAVE, verific daca trebuie sa salvez imaginea in format text sau ascii si in functie de acest lucru
redeschid fisierul cu "wb" sau "wt", pentru a putea printa headerul salvat in structura si matricea actuala in fisier.

7. La comanda APPLY, ma asigur ca doar imaginilor color le pot fi aplicate acest efect si cu ajutorul variabilelor de selectie
si a lungimii, respectiv latimii imaginii verific pixelii carora le pot aplica efectul(care au vecini), si daca e posibil
le aplic kernelul potrivit fiecarui tip de efect iar dupa ce rotunjesc bag noua valoare in imagine. Calculele se fac cu 
ajutorul matrcii auxiliare pentru ca calculele sa fie corecte iar dupa ce aplic efectul copiez matricea imagine in ce auxiliara.

8. La comanda HISTOGRAM, ma asigur ca este o imagine alb negru si daca numarul de bin-uri este posibil. Dupa aceea calculez care
este numarul cu cel mai mare numar de aparitii in imagine, iar dupa cu 4 for-uri , calculez pentru fiecare element de cate ori apare
si folosind formula calculez cate stelute am nevoie la fiecare coloana.

9. La comanda EQUALIZE, ma asigur ca imaginea este alb-negru, iar dupa aceasta creez un vector de frecventa in care pun
numarul de aparitii a fiecarei valoare in imagine, dupa cu ajutorul acestui vector si a formulei, calculez noua valoare a pixelului,
si aici pentru calcule folosesc matricea aux. iar dupa ce calculez noile valori ale pixelilor copiez imaginea in matrciea auxiliara.

10. La comanda ROTATE, ma asigur ca selectia este patrata sau daca este selectata toata poza. Daca este selectata toata poza
reatribui valorile necesare matricii, cum ar fi latimea, lungimea si datele din structura pentru header, dupa care pentru fiecare
caz de rotire aplic formulele pentru rotirea pixelilor. Rotirea intre imaginiile color sau alb-negru nu difera foarte mult ca logica.
Daca selectia e patrata aplic formulele pentru rotire in fiecare caz. La sfarist copiez matricea imagine in cea auxiliara, si daca este
cazul sa fie selectata imaginea completa pentru rotire eliberez si realoc memorie pentru imagine, respectiv matricea auxiliara cu
noile hight si width, inafara de cazul in care trebuie sa rotesc imagine +-180, +-360, caci latimea si inaltimea imaginii nu se schimba.

11. La comanda EXIT eliberez toata meoria alocata dinamic.
