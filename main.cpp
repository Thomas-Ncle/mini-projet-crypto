#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;

#define BLACK_JOKER 53
#define RED_JOKER 54


int letterToInt(char letter)
{
    return letter - 'A' + 1; // ASCII(A) = 65
}

char intToLetter(int number)
{
    return number + 'A' - 1;
}

// Ask the message and remove specials chars and spaces
string askMessage()
{
    string input, message;

    cout << "Entrez le message à chiffrer : " << endl;
    getline(cin, input);


    for (char c : input)
    {
        if (isalnum(c))
            message += toupper(c);
    }
    return message;
}

/* Les opérations pour obtenir le flux de clefs */
void reculJokerNoir(vector<int>& paquet)
{
    if (paquet.back() == BLACK_JOKER)
    {
        // Si dernière position
        paquet.erase(paquet.begin() + 53);
        paquet.insert(paquet.begin() + 1, 53);
    }
    else
    {
        auto it = find(paquet.begin(), paquet.end(), 53);
        int index = distance(paquet.begin(), it); // Position du joker
        swap(paquet[index], paquet[index + 1]);
    }
}

void reculJokerRouge(vector<int>& paquet)
{
    if (paquet.back() == RED_JOKER) //  Si dernière position
    {
        paquet.erase(paquet.begin() + 53);
        paquet.insert(paquet.begin() + 2, 54); // il passe en troisième position
    }
    else if (paquet.at(52) == RED_JOKER) // Si avant dernière position
    {
        paquet.erase(paquet.begin() + 52); // 53-1 (debut=0)
        paquet.insert(paquet.begin() + 1, 54); // il passe en deuxième position
    }
    else
    {
        auto it = find(paquet.begin(), paquet.end(), RED_JOKER);
        int index = distance(paquet.begin(), it); // Position du joker
        swap(paquet[index], paquet[index - 2]);
        swap(paquet[index], paquet[index - 1]);
    }
}

void coupeDouble(vector<int>& paquet)
{
    // Trouve les positions des jokers dans le paquet
    auto joker_rouge = std::find(paquet.begin(), paquet.end(), RED_JOKER);
    auto joker_noir = std::find(paquet.begin(), paquet.end(), BLACK_JOKER);

    // Les indices des jokers dans le paquet
    int pos1 = std::distance(paquet.begin(), joker_rouge);
    int pos2 = std::distance(paquet.begin(), joker_noir);

    std::vector<int> auDessus, entre, enDessous;

    // Si pos1 < pos2, traiter normalement
    if (pos1 < pos2)
    {
        auDessus = std::vector<int>(paquet.begin(), paquet.begin() + pos1); // Avant le premier joker
        entre = std::vector<int>(paquet.begin() + pos1, paquet.begin() + pos2 + 1); // Entre les deux jokers (incluant)
        enDessous = std::vector<int>(paquet.begin() + pos2 + 1, paquet.end()); // Après le deuxième joker
    }
    else
    {
        // Sinon, inverser les rôles dans la logique
        auDessus = std::vector<int>(paquet.begin(), paquet.begin() + pos2); // Avant le premier joker (pos2)
        entre = std::vector<int>(paquet.begin() + pos2, paquet.begin() + pos1 + 1); // Entre les deux jokers (incluant)
        enDessous = std::vector<int>(paquet.begin() + pos1 + 1, paquet.end()); // Après le deuxième joker (pos1)
    }

    // Recomposer le paquet dans le bon ordre
    paquet.clear();
    paquet.insert(paquet.end(), enDessous.begin(), enDessous.end()); // Partie après le dernier joker
    paquet.insert(paquet.end(), entre.begin(), entre.end()); // Partie avec les jokers et entre eux
    paquet.insert(paquet.end(), auDessus.begin(), auDessus.end()); // Partie avant le premier joker
}

void coupeSimple(vector<int>& paquet)
{
    int n = paquet.back();
    if (n >= 53) n = 53; // peu importe la couleur du joker

    vector<int> coupe1(paquet.begin(), paquet.begin() + n);
    vector<int> coupe2(paquet.begin() + n, paquet.end() - 1);
    int dernier = paquet.back();
    // Réorganiser le paquet
    paquet.clear();
    paquet.insert(paquet.end(), coupe2.begin(), coupe2.end()); // Ajouter d'abord les cartes de la coupe2
    paquet.insert(paquet.end(), coupe1.begin(), coupe1.end()); // Ajouter les cartes coupées
    paquet.push_back(dernier); // Ajouter la dernière carte
}

void lecture(vector<int>& paquet, vector<char>& flux)
{
    // Etape 1 : lire la première carte du paquet
    int n = paquet.front();
    //cout << "Etape 1 : " << n << endl;
    // Etape 2 : Compter n cartes (la 1 ère carte incluse)
    int m = paquet.at(n);
    //cout << "Etape 2 : " << m << endl;
    // Etape 3 : vérifier si c'est un Joker ou pas
    if (m == BLACK_JOKER || m == RED_JOKER)
    {
        reculJokerNoir(paquet);
        reculJokerRouge(paquet);
        coupeDouble(paquet);
        coupeSimple(paquet);
        lecture(paquet, flux);
    }
    else
    {
        //cout << "Etape 3 : je suis la " << endl;
        // Etape 4 : crypter les cartes
        // Si m > 26
        if (m > 26)
        {
            m = m - 26;
        }
        flux.push_back(intToLetter(m));

        reculJokerNoir(paquet);
        reculJokerRouge(paquet);
        coupeDouble(paquet);
        coupeSimple(paquet);
       /* for (char c : flux)
            cout << "Etape 4 : " << c << " ";
        cout << endl;*/
    }
}

string cryptage(string message, vector<char>& flux)
{
    string crypted_message = "";
    for (size_t i = 0; i < message.size(); i++)
    {
        // Obtenir la valeur numérique des lettres
        int valeurMessage = letterToInt(message[i]); // Convertit la lettre du message en nombre
        int valeurCle = letterToInt(flux[i]); // Convertit la lettre du flux en nombre

        // Ajouter les valeurs (modulo 26 pour rester dans l'alphabet)
        int valeurCryptee = (valeurMessage + valeurCle - 1) % 26 + 1;

        // Convertir la valeur en une lettre et l'ajouter au message crypté
        crypted_message += intToLetter(valeurCryptee);
    }
    return crypted_message;
}

string decryptage(string message_codee, vector<char>& flux)
{
    string decrypted_message = "";
    for (size_t i = 0; i < message_codee.size(); i++)
    {
        // Obtenir les valeurs numériques des lettres
        int valeurCryptee = letterToInt(message_codee[i]); // Lettre du message crypté en nombre
        int valeurCle = letterToInt(flux[i]);              // Lettre du flux en nombre

        // Soustraire les valeurs (modulo 26 pour rester dans l'alphabet)
        int valeurDecryptee = (valeurCryptee - valeurCle + 26) % 26; // Ajout de 26 pour éviter les nombres négatifs
        if (valeurDecryptee == 0) valeurDecryptee = 26; // Ajuster pour que 0 devienne 26 (Z)

        // Convertir la valeur en une lettre et l'ajouter au message décrypté
        decrypted_message += intToLetter(valeurDecryptee);
    }
    return decrypted_message;
}

void initPaquet(vector<int>& paquet)
{
    string s;
    cout << "Entrer " << 54 << " nombres separes par des espaces (ordre du paquet) :" << endl;
    getline(cin, s); // Récupère la ligne et la stocke dans le string

    stringstream iss(s); // Récupère la séquence de characters de s
    int num;

    paquet.clear(); // Assure que le vecteur est vide avant de remplir
    for (int i = 0; i < 54; i++)
    {
        if (!(iss >> num))
        {
            cerr << "Nombre insuffisant entre ! Il faut " << 54 << " nombres." << endl;
            exit(1);
        }
        paquet.push_back(num); // Ajouter le nombre au vecteur
    }
}

int main()
{
    vector<int> paquet;
    vector<char> flux;

    initPaquet(paquet);
    cout<< u8"\033[2J\033[1;1H"; // console.clear (work only with linux)
    string message = askMessage();

    reculJokerNoir(paquet);
    reculJokerRouge(paquet);
    coupeDouble(paquet);
    coupeSimple(paquet);

    cout << "Affichage du paquet : " << endl;
    for (int a : paquet)
        cout << a << " ";
    cout << endl;

    // Flux généré en fct de la taille du msg
    for (int i = 0; i < message.size(); i++)
        lecture(paquet, flux);

    cout << "Flux : ";
    for (char letter : flux)
        cout << letter << " ";
    cout << endl;

    string message_codee = cryptage(message, flux);
    cout << "Message chiffré : " << message_codee << endl;
    cout << "Message déchiffré : " << decryptage(message_codee, flux) << endl;

    return 0;
}
