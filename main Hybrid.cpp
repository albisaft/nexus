#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#include <math.h>

#include <stdlib.h>
#include <cstdlib>
#include <ctype.h>
#include <unistd.h>
#include <cstring>

#include <time.h>

#include <regex>

#include <set>
#include <map>
#include <algorithm>

#include <random>
#include <cstdint>
#include <cassert>

#include "basis Hybrid.h"

using namespace std;

#include <time.h>

ofstream spoken("spoken");


#include "bp.cpp"

string read_file_str(string filename) {
    fstream f(filename, fstream::in);
    string s;
    getline(f, s, '\0');
    f.close();
    return s;
}

const char *fen_char[] = {  // bei writ()
    "S_Tr",   "S_Kr",   "S_K",    "S_D",    "S_T",  "S_L",  "S_P",
    "S_B",    "S_Bu",   "S_Bp_r", "S_Bp_l", "S_Bx", "LEER", "W_Bx",
    "W_Bp_l", "W_Bp_r", "W_Bu",   "W_B",    "W_P",  "W_L",  "W_T",
    "W_D",    "W_K",    "W_Kr",   "W_Tr",   "RAND"
};

void read_fen(Spielfeld sp, string s) {
    std::regex rgx_fen("(\\\\d|[A-Za-y0-9])");

    int ii = 0;
    int x;
    int pos;
    int figur;
    for (std::sregex_iterator i =
                std::sregex_iterator(s.begin(), s.end(), rgx_fen);
            // Figuren finden
            i != std::sregex_iterator();

            ++i) {
        cout << "@";
        std::smatch m = *i;
        // if (i != s.end())
        //  x = std::distance(figuren_char, std::find(fen_char, fen_char + 27,
        //  m[1].str()));
        // else
        //  atoi (
        cout << ii << ":" << x << " " << flush;
        // Positionen in der Liste der Figurennamen finden
        pos = 120 - (21 + ((int)(ii) / 8) * 10 + 8 - ii % 8);
        // Positionen auf dem Feld errechnen, mehrere Spiegelungen

        figur = x - figurenanzahl;
        // Tatsaechliche Zahl fuer die Figuren errechnen
        grundfeld[pos] = x - figurenanzahl;
        // Auf Grundfeld setzen

        ii++;
    }
    return;
}

template <class T, size_t N>
constexpr size_t size(T(&)[N]) {
    return N;
}



int main(int argc, char **argv) {
    int index;
    char *fvalue = NULL;
    string svalue;
    int ladefeld[120] = {RAND };
    opterr = 0;
    vector<string> zuege;
    std::regex rgx_feld("(\\.|[A-Za-y]{3})");
    std::smatch matches;
    string s;
    int c;
    int ii = 0;
    int pos = 0;
    int figur;
    bool geladen = false;
    bool allein = false;
    bool _user = false;

    bool exit = false;
    int pos1;
    int pos2;
    int wert;
    int zug_nummer = 1;
    timeline = 0;
    clock_t t1, t2, t0;
    t0 = clock();


    // Kommandozeilenargumente

    while ((c = getopt(argc, argv, "aus:f:")) != -1)
        switch (c) {
        case 'f': {  // Farbe setzen
            fvalue = optarg;
            eigene_farbe = atoi(fvalue);
        }
        break;
        case 'u': {  // User modus
            _user = true;
        }
        break;
        case 'a': {  // Allein mit sich spielen
            allein = true;
        }
        break;

        case 's': {  // Datei einlesen, aus Kommandozeile kopiert
            *grundfeld = *ladefeld;
            svalue = optarg;
            cout << svalue;
            s = read_file_str(svalue);
            cout << "|" << s << "|";
            // Datei lesen
            cout << s;
            for (std::sregex_iterator i =
                        std::sregex_iterator(s.begin(), s.end(), rgx_feld);
                    // Figuren finden
                    i != std::sregex_iterator(); ++i) {
                cout << "@";
                std::smatch m = *i;
                int x = std::distance(
                            figuren_char,
                            std::find(figuren_char, figuren_char + 27, m[1].str()));
                cout << ii << ":" << x << " " << flush;
                // Positionen in der Liste der Figurennamen finden
                pos = 120 - (21 + ((int)(ii) / 8) * 10 + 8 - ii % 8);
                // Positionen auf dem Feld errechnen, mehrere Spiegelungen

                figur = x - figurenanzahl;
                // Tatsaechliche Zahl fuer die Figuren errechnen
                grundfeld[pos] = x - figurenanzahl;
                // Auf Grundfeld setzen

                ii++;
            }
            cout << " geladenes Spielfeld: \n";
            disp(grundfeld);
            geladen = true;
        }
        break;
        case '?':
            if (optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort();
        }

    {
        for (int i = 0; i < ende + 2; i++) {
            testbrett[i] = new feldtyp;
            testspiel[i] = new Spielfeld();
        }
    }

    zobrist_initialisieren();
    memset(tt_tabelle, 0, sizeof(tt_tabelle));

    Spielfeld spiel(grundfeld, +1, 0);


    // UCI Protokoll
    if (!_user)
        do {

            string command;
            cin >> command;

            if (spoken.is_open()) {
                try {
                    spoken << command << flush << "\n";
                } catch (const std::ios_base::failure &e) {
                    std::cout << "Caught std::ios_base::failure: " << typeid(e).name()
                              << std::endl;
                } catch (const std::exception &e) {
                    std::cout << "Caught std::exception: " << typeid(e).name()
                              << std::endl;
                }
            } else {
                cout << "Unable to open file";
            }

beginning:

            if (command == "-user") {
                status = user;
                break;
            }
            if (command == "-show") {
                spiel.disp();
            }
            if (command == "-show_csv") {
                spiel.disp_cleanest();
            }
            // wichtige Initkommandos - wo man antworten muss

            if (command == "uci") {
                cout << "id name NEXUS 260220 PingPong Fix\n";
                cout << "id author Albrecht Fiebiger & Stefan Werner\n";
                cout << "uciok\n";
            }

            if (command == "isready") {
                cout << "readyok\n";
            }
            if (command == "quit") {
                return 0;
            }

            // Spielkommandos...
            if (command == "position") {
                cin >> command;
                spoken << command << flush << "\n";

                if (command == "startpos") {
                    spiel.setPos(grundfeld, +1, 0, zuege);
                    zug_nummer = 1;
                    letzter_zug_weiss = {0,0,false};
                    letzter_zug_weiss_prev = {0,0,false};
                    letzter_zug_schwarz = {0,0,false};
                    letzter_zug_schwarz_prev = {0,0,false};


                    cin >> command;
                    spoken << command << flush << "\n";

                    if (command == "moves") {
                        while (true) {
                            cin >> command;
                            spoken << command << flush << "\n";



                            denkpaar _zug;
                            int i;
                            int j;
                            for (i = 0; i <= 7; i++) {
                                if ((command[0] == buchstaben1[i]) ||
                                        (command[0] == buchstaben2[i])) {
                                    for (j = 0; j <= 7; j++) {
                                        if (command[1] == zahlen[j]) {
                                            _zug.z.pos.pos1 = 21 + j * 10 + i;
                                        }
                                    }
                                }
                            }
                            for (i = 0; i <= 7; i++) {
                                if ((command[2] == buchstaben1[i]) ||
                                        (command[2] == buchstaben2[i])) {
                                    for (j = 0; j <= 7; j++) {
                                        if (command[3] == zahlen[j]) {
                                            _zug.z.pos.pos2 = 21 + j * 10 + i;
                                        }
                                    }
                                }
                            }

                            spiel.makeZugstapel();
                            bool falsch = true;
                            for (i = 0; i < spiel.n; i++) {
                                if ((zugstapel[spiel.Stufe][i].z.id == _zug.z.id)) {
                                    denkpaar played = zugstapel[spiel.Stufe][i]; // Zug sichern, BEVOR realer_zug die Stufe ändert
                                    int mover = spiel.Farbe; // Wer zieht jetzt (vor realer_zug)

                                    spiel.realer_zug(played, zuege);

                                    // letzten Zug korrekt setzen (aus 'played', nicht mehr aus zugstapel[…]!)
                                    if (mover == 1) {
                                        letzter_zug_weiss_prev = letzter_zug_weiss;
                                        letzter_zug_weiss.pos1 = played.z.pos.pos1;
                                        letzter_zug_weiss.pos2 = played.z.pos.pos2;
                                        letzter_zug_weiss.kill = played.kill;
                                    } else {
                                        letzter_zug_schwarz_prev = letzter_zug_schwarz;
                                        letzter_zug_schwarz.pos1 = played.z.pos.pos1;
                                        letzter_zug_schwarz.pos2 = played.z.pos.pos2;
                                        letzter_zug_schwarz.kill = played.kill;
                                    }



                                    spiel.zug_reset();
                                    falsch = false;
                                    break;
                                }
                            }
                            //for (auto e : zuege) cout << e << " ";
                            if (falsch == true)
                                goto beginning;  // eine Goto-Anweisung; Wehe dem
                            // Spaghettiprogramm!
                            zug_nummer += 1;
                        }
                    }
                }
            }

            if (command == "go") {
                int Restzeit_W;
                int Restzeit_S;
                int Restzeit;

                for (cin >> command; command != "wtime" && command != "btime"; ) {
                    cin >> command;
                }

                if (command == "wtime") {
                    cin >> Restzeit_W;
                    spoken << command << flush << "\n";
                    cin >> command;
                }

                if (command == "btime") {
                    cin >> Restzeit_S;
                    spoken << command << flush << "\n";
                }


                if (spiel.Farbe == 1) {
                    Restzeit = Restzeit_W;
                } else
                    Restzeit = Restzeit_S;

                t1 = clock();
                spiel.setStufe(0);

                for(int i=21; i<99; i++) {
                    for(int j=21; j<99; j++) {
                        historyMoves[i][j] = 0;
                    }
                }

                for(int i=0; i<100; i++) {
                    killerMoves[i][0].z.id = 0;
                    killerMoves[i][1].z.id = 0;
                }

                int letzterWert = 0;  //Score aus der letzten Iteration
                bool habeLetzten = false;
                bool zeitAbgelaufen = false;

                for (int _stopp = 1;; _stopp++) {

                    int alpha, beta;
                    int fenster = 100;  // Startbreite Aspiration Windows in Centipawn

                    // Ab Tiefe 3: Enges Fenster um den letzten Score
                    if (habeLetzten && _stopp >= 3) {
                        alpha = letzterWert - fenster;
                        beta  = letzterWert + fenster;
                    } else {
                        alpha = -MAX_WERT;
                        beta  = MAX_WERT;
                    }

                    // Rufe die Suche mit dem aktuellen Fenster auf
                    wert = bp(spiel, spiel.Farbe, alpha, beta, 0, _stopp, 1);

                    // Bei Fail: Fenster schrittweise verdoppeln
                    while (habeLetzten && _stopp >= 3 && (wert <= alpha || wert >= beta)) {
                        fenster *= 2;

                        // Sicherheitsnetz: Ab 800 volles Fenster
                        if (fenster >= 800) {
                            alpha = -MAX_WERT;
                            beta  = MAX_WERT;
                        } else {
                            alpha = letzterWert - fenster;
                            beta  = letzterWert + fenster;
                        }

                        // Zeitkontrolle VOR dem Re-Search
                        double elapsed_ms = 1000.0 * (double)(clock() - t1) / CLOCKS_PER_SEC;
                        int Zeitfaktor = (zug_nummer <= 120) ? (60 - zug_nummer / 4) : 30;
                        if (elapsed_ms * 1.7 >= (double)Restzeit / (double)Zeitfaktor) {
                            stopp_tatsaechlich = _stopp;
                            zeitAbgelaufen = true;
                            break; // aus while, danach auch aus der for-Schleife  // Keine Zeit mehr für Re-Search
                        }

                        wert = bp(spiel, spiel.Farbe, alpha, beta, 0, _stopp, 1);
                        if (wert == MAX_WERT) {
                            // Matt gefunden, Iteration kann beendet werden
                            break;
                        }
                    }

                    if (zeitAbgelaufen) {
                        break; // äußere for(_stopp)-Schleife beenden
                    }

                    letzterWert = wert;
                    habeLetzten = true;

                    // Zeitkontrolle nach der Iteration
                    int Zeitfaktor = 1;
                    if (zug_nummer <= 120)
                        Zeitfaktor = 60 - zug_nummer / 4;
                    else
                        Zeitfaktor = 30;

                    double elapsed_ms = 1000.0 * (double)(clock() - t1) / CLOCKS_PER_SEC;
                    if (elapsed_ms * 1.7 >= (double)Restzeit / (double)Zeitfaktor || wert == MAX_WERT) {
                        stopp_tatsaechlich = _stopp;
                        break;
                    }
                }

                t2 = clock();
                timeline = (double)(timeline * (zug_nummer - 1) / zug_nummer +
                                    (t2 - t1) / zug_nummer);
                int spez;

                exit = true;
                switch (spiel.check_end(zuege)) {
                case matt: {
                    cout << "Verloren\n";
                    break;
                }
                case patt: {
                    cout << "Patt\n";
                    break;
                }
                case remis: {
                    cout << "Remis\n";
                    break;
                }
                case schachmatt: {
                    cout << "Gewonnen\n";
                    break;
                }
                case nothing: {
                    exit = false;
                    break;
                }
                default: {
                    cout << "Unbekanntes Spielende!";
                    break;
                }
                }

                cout << "info depth " << stopp_tatsaechlich << " score cp " << wert/1.55 << " pv " << grundfeld_bezeichnungen[bester_zug[0].z.pos.pos1]
                     << grundfeld_bezeichnungen[bester_zug[0].z.pos.pos2] << " " << grundfeld_bezeichnungen[bester_zug[1].z.pos.pos1]
                     << grundfeld_bezeichnungen[bester_zug[1].z.pos.pos2] << " " << grundfeld_bezeichnungen[bester_zug[2].z.pos.pos1]
                     << grundfeld_bezeichnungen[bester_zug[2].z.pos.pos2] << " " << grundfeld_bezeichnungen[bester_zug[3].z.pos.pos1]
                     << grundfeld_bezeichnungen[bester_zug[3].z.pos.pos2] << " " << grundfeld_bezeichnungen[bester_zug[4].z.pos.pos1]
                     << grundfeld_bezeichnungen[bester_zug[4].z.pos.pos2] << "\n";
                cout << "bestmove " << grundfeld_bezeichnungen[bester_zug[0].z.pos.pos1]
                     << grundfeld_bezeichnungen[bester_zug[0].z.pos.pos2] << "\n";

                if (spiel.Farbe == 1) {
                    letzter_zug_weiss_prev = letzter_zug_weiss;
                    letzter_zug_weiss.pos1 = bester_zug[0].z.pos.pos1;
                    letzter_zug_weiss.pos2 = bester_zug[0].z.pos.pos2;
                    letzter_zug_weiss.kill = bester_zug[0].kill;
                } else {
                    letzter_zug_schwarz_prev = letzter_zug_schwarz;
                    letzter_zug_schwarz.pos1 = bester_zug[0].z.pos.pos1;
                    letzter_zug_schwarz.pos2 = bester_zug[0].z.pos.pos2;
                    letzter_zug_schwarz.kill = bester_zug[0].kill;
                }

                zug_nummer += 1;
            }
        } while (true);


    // Benutzermodus

    do {
        disp(spiel.to_feld());
        writ(spiel.to_feld());

        if (!((zug_nummer == 1) && (eigene_farbe == 1))) {  // Benutzerzug
            bool ok = false;
            do {
                if (allein) {
                    eigene_farbe *= -1;
                    break;
                }
                int spez;
                int n = spiel.zuggenerator();


                cout << "Zug " << zug_nummer << ", von ";
                pos1 = eingabe();
                if (pos1 == 666 || pos2 == 666) {
                    eigene_farbe *= -1;
                    break;
                }
                cout << "nach ";
                pos2 = eingabe();

                if (pos1 == 666 || pos2 == 666) {
                    eigene_farbe *= -1;
                    break;
                }

                for (int i = 0; i < n; i++) {
                    denkpaar& mz = zugstapel[spiel.Stufe][i]; // globales Move-Array
                    if ((mz.z.pos.pos1 == pos1) &&
                            (mz.z.pos.pos2 == pos2)) {
                        ok = true;
                        denkpaar played = mz;
                        int mover = spiel.Farbe; // Farbe VOR dem Zug merken
                        spiel.realer_zug(played, zuege);


                        if (mover == 1) {
                            letzter_zug_weiss_prev = letzter_zug_weiss;
                            letzter_zug_weiss.pos1 = played.z.pos.pos1;
                            letzter_zug_weiss.pos2 = played.z.pos.pos2;
                            letzter_zug_weiss.kill = played.kill;
                        } else {
                            letzter_zug_schwarz_prev = letzter_zug_schwarz;
                            letzter_zug_schwarz.pos1 = played.z.pos.pos1;
                            letzter_zug_schwarz.pos2 = played.z.pos.pos2;
                            letzter_zug_schwarz.kill = played.kill;
                        }

                        //zuege_append(zuege, spiel.hash());
                        //if (zuege_wied(zuege)) exit = true;
                        spiel.zug_reset();
                        break;
                    }
                }
                if (ok == false)
                    cout << "\nUnmoegliche Eingabe, vertippt?\n";

            } while (!ok);
        }


        bewertet = 0;
        t1 = clock();

        spiel.setStufe(0);
        //  int devwert = 0;
        for (int _stopp = 1; ; _stopp++) {



            cout << "Suchtiefe " << _stopp << "\n";

            wert = bp(spiel, spiel.Farbe, -MAX_WERT, MAX_WERT, 0, _stopp, 1);

            double elapsed_ms_user = 1000.0 * (double)(clock() - t1) / CLOCKS_PER_SEC;
            if (elapsed_ms_user >= 300.0 && _stopp >= stopp)
                break;
        }


        t2 = clock();
        timeline = (double)(timeline * (zug_nummer - 1) / zug_nummer +
                            (t2 - t1) / zug_nummer);
        int spez;

        cout << "\nmove " << (double)zug_nummer / 2 << ": "
             << ((spiel.Farbe > 0) ? "white" : "black") << ", "
             << "time " << (double)(t2 - t1) / CLOCKS_PER_SEC << "s, "
             << "average " << (double)(timeline) / CLOCKS_PER_SEC << "s, "
             << "total " << (double)(clock() - t0) / CLOCKS_PER_SEC << "s, "
             << "evaluations " << bewertet << "\n";

        cout << "result     " << grundfeld_bezeichnungen[bester_zug[0].z.pos.pos1]
             << " => " << grundfeld_bezeichnungen[bester_zug[0].z.pos.pos2] << "\n";

        spiel.realer_zug(bester_zug[0], zuege);


        exit = true;
        switch (spiel.check_end(zuege)) {
        case matt: {
            cout << "Verloren\n";
            break;
        }
        case patt: {
            cout << "nn = " << spiel.nn;
            cout << "Patt\n";
            break;
        }
        case remis: {
            cout << "Remis\n";
            break;
        }
        case schachmatt: {
            cout << "Gewonnen/n";
            break;
        }
        case nothing: {
            //cout << "weiter\n";
            exit = false;
            break;
        }
        default: {
            cout << "Unbekanntes Spielende!";
            break;
        }
        }

        spiel.zug_reset();
        zug_nummer++;

    } while (!exit);

    cout << "\n\n							"
         "ENDE\n";

    spoken.close();
    return 0;
}
