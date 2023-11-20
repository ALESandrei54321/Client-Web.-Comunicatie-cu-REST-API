# Tema 4 - Client Web. Comunicatie cu REST API

----------Functionalitate----------

Clientul are implementate 9 comenzi pe care le poate interpreta:

1. "register"
* dupa introducerea acestei comenzi, clientul va astepta introducerea de la tastatura a numelui si parolei dorite
* validarea datelor se reduce la faptul ca stringurile de input sa nu fie goale sau sa nu contina orice fel de whitespace.

2. "login"
* dupa introducerea acestei comenzi, clientul va astepta introducerea de la tastatura a numelui si parolei asociate contului dorit
* validarea datelor se reduce la faptul ca stringurile de input sa nu fie goale sau sa nu contina orice fel de whitespace
* serverul va da eroare atat pentru cont inexistent cat si pentru parola gresita
* daca logarea are loc, clientul va retine un cookie trimis de catre server. 

3. "enter_library"
* dupa introducerea acestei comenzi, clientul va verifica existenta cookie-ului descris mai sus
* daca utilizatorul este logat, serverul va genera un token JWT pe care clientul il retine.

4. "get_books"
* dupa introducerea acestei comenzi, clientul va verifica existenta tokenului descris mai sus
* daca nu apar erori, clientul va afisa toate cartiile din biblioteca.

5. "get_book"
* dupa introducerea acestei comenzi, clientul va astepta introducerea de la tastatura a id-ului asociat cartii dorite
* daca id-ul este un numar, clientul interogheaza serverul si, daca exista o carte cu acel id, afiseaza toate datele despre aceasta.

6. "add_book"
* dupa introducerea acestei comenzi, clientul va astepta introducerea de la tastatura a datelor asociate cartii ce va fi introdusa (titlu, autor, gen, publisher, numar de pagini)
* validarea datelor se reduce la faptul ca stringurile de input sa nu fie goale. De asemenea, campul de page_count trebuie sa fie un numar
* daca nu exista erori cu datele de intrare, cartea va fi adaugata la biblioteca noastra virtualas si serverul ii va asocia un id unic.

7. "delete_book"
* dupa introducerea acestei comenzi, clientul va astepta introducerea de la tastatura a id-ului asociat cartii dorite pentru a fi stearsa
* daca id-ul este un numar, clientul interogheaza serverul si, daca exista o carte cu acel id, o elimina din lista de carti din biblioteca.

8. "logout"
* dupa introducerea acestei comenzi, clientul va verifica daca utilizatorul este logat cu ajutorul campului de login_cookie
* daca este logat, clientul sterge atat cookiul asociat conectarii, cat si tokenul de sesiune (daca acesta exista).

9. "exit"
* dupa introducerea acestei comenzi, clientul se inchide.

----------Functii auxiliare-----------
Pentru implementarea functionalitaiilor descrise mai sus, am creat cateva functii auxiliare de trei tipuri:
- de validare al datelor: isNumber, containsSubstring, containsWhitespace
- de extragere de date din raspunsul serverului: extract_cookies, extractToken
- de afisare: printBooks, printBookData

Totodata, am adaugat la scheletul asociat temei (laboratorul 9) inca o functie compute_delete_request si am modificat usor implementarile descrise in laborator pentru a permite lucrul cu string-urile din C++. 

----------Libraria JSON-----------
Am ales libraria nlohmann::json pentru parsarea mesajelor trimise si primite de la server, motivul principal fiind ca mi s-a parut mai usor de utilizat decat varianta de C. Acesta este motivul pentru care si tema este scrisa in C++.

----------Rulare----------
Tema poate fi rulata cu ajutorul Makefile-ului inclus.
- make run : compileaza si executa ./client
- make clean: sterge fisierele obiect si client













