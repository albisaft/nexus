using namespace std;

// TYPEN
struct wandel {
    int pos1;
    int fig;
};
union zug {
    struct {
        char pos1 : 8;
        char pos2 : 8;
    }   pos;
    int id : 16;
};
struct denkpaar {
    zug    z;
    int    farbe;
    int    figur;
    int    spezial;
    int    bewertung;
    int    i;
    wandel verwandelung[20];
    int    nw;
    bool   reckoning;
    bool   kill;
    double   order;
};

struct figurtyp {
    int  pos1;
    int  pos2;
    bool schach;
    bool bewegt;
};
struct feldtyp  {
    int feld[120];
    int x;
};

double historyMoves[120][120];
denkpaar killerMoves[100][2]; // 2 Killer pro Tiefe

int eigene_farbe = 1;  // Sicht des Computers --1-0
int stopp        = 5;  // normale Suchtiefe, wird ver‚Ä8ndert
int stopp_tatsaechlich = 0;

const int ende   = 15; // maximale Suchtiefe
int sortiertiefe = 15; // Sortiertiefe - wieviele Z¬?ge werden sortiert
//int figurenwert_weiss = 0;
//int figurenwert_schwarz = 0;
int figurenwert = 0;

int ttMoveId[ende + 2];

int    KooIch    = 365;  //???
int    KooEr     = 50;    //???
double AttackIch = 3.5;
double AttackEr  = 3;
int    DefIch1   = 80;
int    DefIch2   = 200;
int    DefEr1    = 60;
int    DefEr2    = 170;
double AttDame   = 0.11;
int    MobTurm   = 15;
double AttTurm   = 0.5;
double MobLau    = 17.5;
double AttLau    = 0.6;
double AttSpr    = 0.55;
double AttBau    = 3.5;
double AttKoe    = 2;
double KSafety   = 800; // ??
int    Kontrolle = 120;
double K_Angriff_Turm = 0.5;
double K_Angriff_Laeufer = 0.25;
double K_Angriff_Springer = 0.25;
int    Figurensicherheit = 180;
int    IsolaniScore = 11;
int    FreibauerScore = 25;

enum state { user, uci, gone, position };
state status = uci;

// const char _DEFAULT_PATH[] = "partie";

const int figurenanzahl = 12; // bei disp()
bool killFlag           = true;
const int MAX_WERT      = 99999999;

extern vector<string> stellungsHistorie;


enum howitends      { matt       = -MAX_WERT, patt = -1, remis = 0, schaach = 1,
                      schachmatt = +MAX_WERT, nothing
                    };
enum espezial           { NICHTS = 0, SCHACH = 1, UNRUH = 2 };
enum spiel_status   { Eroeffnung, Mittelspiel, Spaetspiel, Endspiel };
const char *spiel_status_namen[] = {
    "Eroeffnung", "Mittelspiel", "Mittel-Endspiel", "Endspiel"
};
enum figuren            { // bei disp()
    S_Tr = -12, S_Kr = -11,
    S_K  = -10, S_D, S_T, S_L,
    S_P, S_B, S_Bu, S_Bp_r, S_Bp_l, S_Bx,
    LEER                                                     = 0,
    W_Bx, W_Bp_l, W_Bp_r, W_Bu, W_B, W_P, W_L, W_T, W_D, W_K = 10, W_Kr = 11,
    W_Tr                                                     = 12,
    RAND                                                     = 13
};

const char *figuren_char[] = { // bei disp()
    "tmr", "knr", "kon", "dam", "tum", "laf", "pdf", "bau", "baU", "bar", "bal",
    "bax",
    ".",
    "BAX", "BAL", "BAR", "BAu", "BAU", "PDF", "LAF", "TUM", "DAM", "KON", "KNR",
    "TMR", "RAND"
};

// bei eingabe()
const char *figuren_intern[] =       { // bei writ()
    "S_Tr",   "S_Kr",     "S_K",      "S_D",      "S_T",      "S_L",      "S_P",
    "S_B",
    "S_Bu",
    "S_Bp_r", "S_Bp_l",   "S_Bx",
    "LEER",
    "W_Bx",   "W_Bp_l",   "W_Bp_r",   "W_Bu",     "W_B",      "W_P",      "W_L",
    "W_T",
    "W_D",
    "W_K",    "W_Kr",     "W_Tr",
    "RAND"
};

char buchstaben1[] =       { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
char buchstaben2[] =       { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };
char zahlen[]      =       { '1', '2', '3', '4', '5', '6', '7', '8' };

const char *grundfeld_bezeichnungen[120] = {
    // Ausgabe der Felder
    "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND",
    "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND",
    "RAND", "a1",   "b1",   "c1",   "d1",   "e1",   "f1",   "g1",   "h1",   "RAND",
    "RAND", "a2",   "b2",   "c2",   "d2",   "e2",   "f2",   "g2",   "h2",   "RAND",
    "RAND", "a3",   "b3",   "c3",   "d3",   "e3",   "f3",   "g3",   "h3",   "RAND",
    "RAND", "a4",   "b4",   "c4",   "d4",   "e4",   "f4",   "g4",   "h4",   "RAND",
    "RAND", "a5",   "b5",   "c5",   "d5",   "e5",   "f5",   "g5",   "h5",   "RAND",
    "RAND", "a6",   "b6",   "c6",   "d6",   "e6",   "f6",   "g6",   "h6",   "RAND",
    "RAND", "a7",   "b7",   "c7",   "d7",   "e7",   "f7",   "g7",   "h7",   "RAND",
    "RAND", "a8",   "b8",   "c8",   "d8",   "e8",   "f8",   "g8",   "h8",   "RAND",
    "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND",
    "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND", "RAND",
    "RAND"
};

int grundfeld[120] = {
    RAND, RAND, RAND, RAND, RAND, RAND,  RAND,  RAND,  RAND,   RAND,
    RAND, RAND, RAND, RAND, RAND, RAND,  RAND,  RAND,  RAND,   RAND,
    RAND, W_Tr, W_P,  W_L,  W_D,  W_Kr,  W_L,   W_P,   W_Tr,   RAND,
    RAND, W_Bx, W_Bx, W_Bx, W_Bx, W_Bx,  W_Bx,  W_Bx,  W_Bx,   RAND,
    RAND, LEER, LEER, LEER, LEER, LEER,  LEER,  LEER,  LEER,   RAND,
    RAND, LEER, LEER, LEER, LEER, LEER,  LEER,  LEER,  LEER,   RAND,
    RAND, LEER, LEER, LEER, LEER, LEER,  LEER,  LEER,  LEER,   RAND,
    RAND, LEER, LEER, LEER, LEER, LEER,  LEER,  LEER,  LEER,   RAND,
    RAND, S_Bx, S_Bx, S_Bx, S_Bx, S_Bx,  S_Bx,  S_Bx,  S_Bx,   RAND,
    RAND, S_Tr, S_P,  S_L,  S_D,  S_Kr,  S_L,   S_P,   S_Tr,   RAND,
    RAND, RAND, RAND, RAND, RAND, RAND,  RAND,  RAND,  RAND,   RAND,
    RAND, RAND, RAND, RAND, RAND, RAND,  RAND,  RAND,  RAND,   RAND
}; // */

// ============================================================
// === ZOBRIST-HASHING und TRANSPOSITION TABLE
// ============================================================

// Zobrist-Tabelle: Zufallszahlen für jede Figur auf jedem Feld
uint64_t zobrist_figuren[27][120];  // [figurentyp + 13][feld]
uint64_t zobrist_seite;              // XOR wenn Schwarz am Zug

// Zobrist-Tabelle einmalig mit Zufallszahlen füllen
void zobrist_initialisieren() {
    std::mt19937_64 rng(123456789);
    for (int figurtyp = 0; figurtyp < 27; figurtyp++) {
        for (int feld = 0; feld < 120; feld++) {
            zobrist_figuren[figurtyp][feld] = rng();
        }
    }
    zobrist_seite = rng();
}

// Hash einer Stellung komplett berechnen
uint64_t zobrist_hash_berechnen(int feld[], int farbe) {
    uint64_t h = 0;
    for (int i = 21; i <= 98; i++) {
        int figur = feld[i];
        if (figur != LEER && figur != RAND) {
            h ^= zobrist_figuren[figur + 13][i];
        }
    }
    if (farbe < 0) {
        h ^= zobrist_seite;
    }
    return h;
}

// --- Transposition Table ---
enum TTFlag : unsigned char {
    TT_LEER  = 0,
    TT_EXAKT = 1,
    TT_ALPHA = 2,  // Obergrenze (fail-low: kein Zug war gut genug)
    TT_BETA  = 3   // Untergrenze (fail-high: Beta-Cutoff)
};

struct TTEintrag {
    uint64_t schluessel;  // Zobrist-Hash zur Verifikation
    int      wert;        // Gespeicherter Score
    int      tiefe;       // Verbleibende Suchtiefe
    TTFlag   typ;         // Art des Eintrags
    int      bpiZugId;    // ID des besten Zugs (für Zugsortierung, später)
};

// Tabellengröße: 2^20 = ca. 1 Million Einträge
const int TT_GROESSE = 1 << 20;
const int TT_MASKE   = TT_GROESSE - 1;

TTEintrag tt_tabelle[TT_GROESSE];




/*           PST - Figuren-Felder-Tabellen                */

double __STARTPUNKTE[120] = { // Weiße Bauern Cuckoo
    RAND, RAND, RAND,  RAND,   RAND,  RAND, RAND, RAND,  RAND,   RAND,
    RAND, RAND, RAND,  RAND,   RAND,  RAND, RAND, RAND,  RAND,   RAND,
    RAND, 0,    0,     0,      0,     0,    0,    0,     0,      RAND,
    RAND, -6,   4,     4,      -15,   -15,  4,    4,     -6,     RAND,
    RAND, -6,   4,     2,      5,     5,    2,    4,     -6,     RAND,
    RAND, -6,   4,     5,      16,    16,   5,    4,     -6,     RAND,
    RAND, -5,   4,     10,     20,    20,   10,   4,     -5,     RAND,
    RAND, 100,   100,   100,    100,   100,	100,   100,  100,   RAND,
    RAND, 200,   200,   200,    200,   200,	200,   200,  200,   RAND,
    RAND, 0,    0,     0,      0,     0,    0,    0,     0,      RAND,
    RAND, RAND, RAND,  RAND,   RAND,  RAND, RAND, RAND,  RAND,   RAND,
    RAND, RAND, RAND,  RAND,   RAND,  RAND, RAND, RAND,  RAND,   RAND
};
double __STARTPUNKTEx2[120] = { // Weiße Springer Pawel
    RAND,  RAND,  RAND,  RAND,  RAND,   RAND,   RAND,   RAND,   RAND,  RAND,
    RAND,  RAND,  RAND,  RAND,  RAND,   RAND,   RAND,   RAND,   RAND,  RAND,
    RAND,  -50,   -40,   -30,   -25,    -25,    -30,    -40,    -50,   RAND,
    RAND,  -35,   -25,   -15,   -10,    -10,    -15,    -25,    -35,   RAND,
    RAND,  -20,   -10,   0,     5,      5,      0,      -10,    -20,   RAND,
    RAND,  -10,   0,     10,    15,     15,     10,     0,      -10,   RAND,
    RAND,  -5,    5,     15,    20,     20,     15,     5,      -5,    RAND,
    RAND,  -5,    5,     15,    20,     20,     15,     5,      -5,    RAND,
    RAND,  -20,   -10,   0,     5,      5,      0,      -10,    -20,   RAND,
    RAND,  -135,  -25,   -15,   -10,    -10,    -15,    -25,    -135,  RAND,
    RAND,  RAND,  RAND,  RAND,  RAND,   RAND,   RAND,   RAND,   RAND,  RAND,
    RAND,  RAND,  RAND,  RAND,  RAND,   RAND,   RAND,   RAND,   RAND,  RAND
};
double __STARTPUNKTEx4[120] = { // Läufer
    RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND,
    RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND,
    RAND, -3,  -4, -10,  -4,  -4, -10,  -4,  -3,  RAND,
    RAND, -3,   4,   2,   4,   4,   2,   4,  -3,  RAND,
    RAND, -3,   3,   6,   8,   8,   6,   3,  -3,  RAND,
    RAND, -3,   1,   6,  10,  10,   6,   1,  -3,  RAND,
    RAND, -3,   1,   4,  10,  10,   4,   1,  -3,  RAND,
    RAND, -3,   1,   4,   4,   4,   4,   1,  -3,  RAND,
    RAND, -3,   1,   1,   1,   1,   1,   1,  -3,  RAND,
    RAND, -3,  -3,  -3,  -3,  -3,  -3,  -3,  -3,  RAND,
    RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND,
    RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND, RAND
};
double __STARTPUNKTEx6[120] = { //Türme Cuckoo
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,
    RAND,	0,  3,  5,  5,  5,  5,  3,  0,		RAND,
    RAND,	-3,  2,  5,  5,  5,  5,  2, -3,RAND,
    RAND,	-2,  0,  0,  2,  2,  0,  0, -2,RAND,
    RAND,	-2,  0,  0,  0,  0,  0,  0, -2,RAND,
    RAND,	0,  0,  0,  0,  0,  0,  0,  0,RAND,
    RAND,	0,  0,  0,  0,  0,  0,  0,  0,RAND,
    RAND,	22, 27, 27, 27, 27, 27, 27, 22,RAND,
    RAND,	8, 11, 13, 13, 13, 13, 11,  8,RAND,
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND
};
double __STARTPUNKTEx8[120] = { //Dame
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,
    RAND,-10, -5,  0,  0,  0,  0, -5,-10,	RAND,
    RAND,-5,  0,  5,  5,  5,  5,  0, -5,RAND,
    RAND,0,  5,  5,  6,  6,  5,  5,  0,RAND,
    RAND,0,  5,  6,  6,  6,  6,  5,  0,RAND,
    RAND,0,  5,  6,  6,  6,  6,  5,  0,RAND,
    RAND, 0,  5,  5,  6,  6,  5,  5,  0,RAND,
    RAND,-5,  0,  5,  5,  5,  5,  0, -5,RAND,
    RAND,10, -5,  0,  0,  0,  0, -5,-10,RAND,
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND
};

double __STARTPUNKTEx10[120] = { //König
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,
    RAND,	24, 24,  9,  0,  0,  9, 24, 24,		RAND,
    RAND,	16, 14,  7, -3, -3,  7, 14, 16,RAND,
    RAND,	4, -2, -5,-15,-15, -5, -2,  4,RAND,
    RAND,	-10,-15,-20,-25,-25,-20,-15,-10,RAND,
    RAND,	-15,-30,-35,-40,-40,-35,-30,-15,RAND,
    RAND,	 -25,-35,-40,-45,-45,-40,-35,-25,RAND,
    RAND,	-22,-35,-40,-40,-40,-40,-35,-22,RAND,
    RAND,	-22,-35,-40,-40,-40,-40,-35,-22,RAND,
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,
    RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND
};

double materialwert[15] = { // Materialwert 1.15
    0,                      // NIL! 0
    100,                    // Startbauer 1
    500,                    // durchgestarteter Startbauer 2
    33.3,                   // en passant Bauer 3
    2250,                   // Superbauer 4
    20,                     // Bauer 5
    66.666667,              // Pferd 6
    57.142857,              // Läufer 7
    75,                   // Turm 8
    133.3333333,            // Dame 9
    3058,                   // König 10
    2780,                   // Rochade König 11
    50,                     // Rochade Turm 12
};


// Zahlen fuer den Zuggenerator, Sprenklereinstellungen
int bewegung[15][15] = { // Anzahl, Richtung, Weite, wohin[richtung]
    { 0, 0, 0   },       // -- unsinn, ignorieren --
    { 0, 1, 10  },       // Bauer vor seinem Start
    { 0, 0, 10  },       // durchgestarteter Startbauer (kann kein en passant)
    { 0, 0, 10  },       // en passant Bauer in Aktion, muss sich umverwandelungn
    { 0, 0, 0   },       // Umwandelungsbauer auf der anderen Grundlinie
    { 0, 0, 10  },       // Bauer
    { 7, 0, 21, 12, 19, 8, -21, -12, -19, -8}, // Pferd
    { 3, 6, 9, 11, -9, -11},             // Laeufer
    { 3, 6, 10, 1, -1, -10},             // Turm
    { 7, 6, 9, 10, 11, 1, -1, -9, -10, -11}, // Dame
    { 7, 0, 9, 10, 11, 1, -1, -9, -10, -11}, // Koenig vor Rochade
    { 7, 0, 9, 10, 11, 1, -1, -9, -10, -11}, // Koenig
    { 3, 6, 10, 1, -1, -10},             // Turm vor Rochade
    { 1, 0, 9, 11},                      // Bauer nach seinem Start
// { 2, 0, 9, 10, 11}, // Koenig
};


// VARIABLEN
int zug_nummer;
denkpaar aktueller_zug[ende];
denkpaar bester_zug[ende];
int __FELD[ende + 2][120];
int Feld[ende + 2][200];
denkpaar zugstapel[ende + 2][200];
denkpaar best_one[ende + 2];
int sort_schema[ende][200];
denkpaar sort_schema_bewertung[ende][200];
int bewertet;               // Maß für die Partieeinheit
int timeline;               // Entscheidung Endspiel oder ErÀÜffnung, Einfluss
// auf Bewertung und Suchtiefe
spiel_status partie_status; // Ereoffnung, Mittelspiel....
double				  kingzone[120];
double				  zugzone_ich[120];
double				  zugzone_du[120];


// FUNKTIONEN
void disp(int feld[120], int form = 0)  {
    cout << "\n";
    int breite = 3;

    cout << "      " << "  >--A--v--B--v--C--v--D--v--E--v--F--v--G--v--H--<\n";
    cout << "      " << "v >-----+-----+-----+-----+-----+-----+-----+-----< v\n" <<
         "      ";

    for (int j = 9; j > 1; j--)  {
        cout << j - 1;

        for (int i = 1; i < 9; i++)  {
            if (feld[j * 10 + i] != RAND)
                switch (form)   {
                case 0: {
                    cout << setw(breite) << " | " << setw(breite) <<
                         figuren_char[feld[j * 10 + i] + figurenanzahl];
                    break;
                }

                case 1: {
                    cout << setw(breite) << " | " << setw(breite) <<
                         feld[j * 10 + i];
                    break;
                }
                }
        }
        cout << setw(breite) << "| " << j - 1 << "\n" << "      " <<
             "^ >-----+-----+-----+-----+-----+-----+-----+-----< ^\n" << "      ";
    }
    cout << "  >--A--+--B--+--C--+--D--+--E--+--F--+--G--+--H--<  \n";
    cout << "\n";
}

void print_zugstapel(int n, denkpaar zugstapel[200])  {
    for (int i = 0; i < n; i++)  {
        cout << figuren_char[zugstapel[i].figur  + figurenanzahl] << ": "
             << int(zugstapel[i].z.pos.pos1) << "(" <<
             grundfeld_bezeichnungen[zugstapel[i].z.pos.pos1] << ")  => "
             << int(zugstapel[i].z.pos.pos2) << "(" <<
             grundfeld_bezeichnungen[zugstapel[i].z.pos.pos2] << ")";

        if (zugstapel[i].nw) {
            cout << " | ";
            int max = zugstapel[i].nw;

            for (int j = 0; j < max; j++)    {
                cout << int(zugstapel[i].verwandelung[j].pos1) << "(" <<
                     grundfeld_bezeichnungen[zugstapel[i].verwandelung[j].pos1] << ")  <= "
                     << figuren_char[zugstapel[i].verwandelung[j].fig  + figurenanzahl];

                if (j < max - 1)
                    cout << ", ";
            }
        }

        cout << "\n";
    }
}

void disp_cleanest(int feld[120])  {
    cout << "\n";

    for (int j = 9; j > 1; j--)  {
        for (int i = 1; i < 9; i++)  {
            if (feld[j * 10 + i] !=
                    RAND)
                cout << figuren_char[feld[j * 10 + i] + figurenanzahl];

            if (j * 10 + i != 28)
                cout << ",";
        }
        cout << "\n";
    }
    cout << "\n";
}

void writ(int feld[120], int form = 0)  {
    ofstream partie("partie.txt", ios::app);

    partie << "\n{\n";
    partie << "RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,\n"
           << "RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,\n";

    for (int j = 2; j < 10; j++)  {
        partie << "RAND, ";

        for (int i = 1; i < 9; i++)  {
            if (feld[j * 10 + i] != RAND)
                switch (form)   {
                case 0: {
                    partie << setw(6) <<
                           figuren_intern[feld[j * 10 + i] + figurenanzahl]
                           << ", ";
                    break;
                }
                }
        }
        partie << "  RAND,";
        partie << "\n";
    }
    partie << "RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,\n"
           << "RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND\n";
    partie << "};";
    partie << "\n";
    partie.close();
}

struct endofinput : public exception {
    const char* what() const throw() {
        return "end of file in input stream!";
    }
};

int eingabe()  {
    bool okay = false;
    int  i, j;

    do {
        cout << "Feld ('.' fuer 'zieh doch selber!'):";
        char buchstabe;
        cin >> buchstabe;

        if (buchstabe == '.')
            return 666;

        char zahl;
        cin >> zahl;

        if (zahl == '.')
            return 666;

        if ((buchstabe == 'x') && (zahl == 'x')) {
            cout << "\n";
            return false;
        }

        for (i = 0; i <= 7; i++)
            if ((buchstabe == buchstaben1[i]) || (buchstabe == buchstaben2[i]))
                for (j = 0; j <= 7; j++) {
                    if (zahl == zahlen[j]) {
                        okay = true;
                        return 21 + j * 10 + i;
                    }
                }

        if (cin.eof())
            throw endofinput();
    } while (!okay);
    return 0;
}

void zuege_append(vector<string>& zuege, string _hash)  {
    zuege.insert(zuege.end(), (_hash));
}

bool zuege_wied(vector<string>& _zuege)  {
    // outputs a list of uniqe words
    set<string> unique(_zuege.begin(), _zuege.end());

    // for (auto element : unique)
    //	cout << element << ' ' << count(_zuege.begin(), _zuege.end(), element)
    // << endl;

    std::map<std::string, int> count;

    for (auto x : _zuege)
        ++count[x];

    int  val = 2;
    auto it  = std::find_if(count.begin(), count.end(),
    [val](const std::pair<std::string, int>& t) -> bool {
        return t.second > val;
    }
                           );

    if (it == count.end())  {
        return false;
    } else {
        cout << "position id " << (it->first)
             << "after moves ";

        int ii = 0;

        for  (auto y : _zuege)  {
            ii += 1;

            if (y == (it->first))
                cout << (double)ii / 2 << " ";
        }


        return true;
    }
}


class Spielfeld  {

public:

    int         zuggenerator();
    inline void add_zug(const int & pos1,
                        const int & pos2,
                        const int & n,
                        const bool& _kill,
                        const int & _figur,
                        const double & order);
    inline void add_verwandelung(const int& farbe,
                                 const int& pos,
                                 const int& was,
                                 const int& n);
    inline bool      test_drohung(int feld[],
                                  int farbe,
                                  int pos);
    inline howitends check_end(vector<string>& _zuege);
    inline howitends last_moves();

    int n;
    int nn;
    bool Z;
    int Stufe;
    int Farbe;
    int spezial;
    int wking = 99;
    int bking = 99;
    int test = 0;

    Spielfeld();
    Spielfeld(int _feld[120],
              int _farbe,
              int _stufe);
    void      zug_reset();

    // Spielfeld (Spielfeld & spiel);
    feldtyp * to_feldtyp(feldtyp *neues);
    feldtyp * to_feldtyp();
    int     * to_feld(int _feld[120]);
    int     * to_feld();
    void      copy(Spielfeld& _spiel);

    void      setFarbe(int _farbe);

    int       getStufe();
    void      setStufe(int i);

    bool      look_richtung_td(const int feld [], const int &farbe, const int &pos, const int &step);
    bool      look_richtung_ld(const int feld [], const int &farbe, const int &pos, const int &step);

    void find_kings();

    void      setPos(int _feld[120],
                     int _farbe,
                     int _stufe,
                     vector<string>& _zuege);
    string    hash();

    void      disp();
    void      disp_cleanest();
    void      write();
    void      print_zugstapel();
    denkpaar* makeZugstapel();

    void zug(denkpaar&);
    void realer_zug(denkpaar&, vector<string>& _zuege);

    void norm_zug(denkpaar&);

    bool schach(int _farbe);

    uint64_t hash_wert = 0;  // Zobrist-Hash der Stellung
};


howitends Spielfeld::check_end(vector<string>& _zuege)  {
    find_kings();
    //print_zugstapel();
    //cout << "STUFE::: "<< this ->getStufe() << " w king: "<< this->wking << " b king: " << this->bking;

    if (this -> wking == 0 || this -> bking == 0)  {
        //disp();
        cout << "He took the king!\n";
        //  test = 1;
        return schachmatt;
    }

    if (this->test_drohung(Feld[this->getStufe()], 1, this->wking))  {
        //cout << "weiss hat schach/n";
        if (Farbe > 0)  {
            //return matt;        // verloren
        } else {
            /*test = 1;*/ return schachmatt;
        }
    }

    if (this->test_drohung(Feld[this->getStufe()], -1, this->bking))  {
        //cout << "schwarz hat schach/n";
        if (Farbe < 0)  {
            //return matt;        // verloren
        } else {
            /*test = 1;*/ return schachmatt;
        }
    }



    if (bester_zug[0].z.pos.pos1 == 0 && bester_zug[0].z.pos.pos2 == 0)  {
        if (Farbe > 0)  {

            if (this->test_drohung(Feld[this->getStufe()], this->Farbe,
                                   this->wking)) {
                /*test = 1;*/ return matt;   // verloren
            }

            if (this->test_drohung(Feld[this->getStufe()], this->Farbe * -1,
                                   this->bking)) {
                /*test = 1;*/ return schachmatt;   // gewonnen
            }
        }


        if (Farbe < 0)  {
            if (this->test_drohung(Feld[this->getStufe()], this->Farbe * -1,
                                   this->wking)) {
                /*test = 1;*/ return schachmatt;   // gewonnen
            }

            if (this->test_drohung(Feld[this->getStufe()], this->Farbe,
                                   this->bking)) {
                /*test = 1;*/ return matt;   // verloren
            }
        }
        return patt;                                               // kein zug  moeglich  (was ist mit remis bei gefesselten

    }

    if (zuege_wied(_zuege))
        return remis;
    else
        return nothing;
}

int gegner;
howitends Spielfeld::last_moves()  {
    // testet, ob, nachdem der Zug gesetzt wurde, noch Schach ist; wenn der
    // Gegner nach seinem Zug noch im Schach steht,
    // hat er einen falschen gemacht
    int mover = -this->Farbe;   // +1 = Weiß hat gezogen, -1 = Schwarz hat gezogen
    int moverKing = (mover > 0) ? this->wking : this->bking;

    // test_drohung erwartet als 'farbe' die "angegriffene" Seite (also den Mover)
    if (this->test_drohung(Feld[this->getStufe()], mover, moverKing)) {
            // illegaler Zug: eigener König steht noch im Schach
        return schaach;
    }
    return nothing;

}

Spielfeld::Spielfeld()  {
    Farbe = 0;
    Stufe = 0;
    {
        for (int i = 0; i < 120; i++)  {
            Feld[Stufe][i] = 0;
        }
    }
}

Spielfeld::Spielfeld(int _feld[120], int _farbe = 0, int _stufe = 0)  {
    setFarbe(_farbe);
    setStufe(_stufe);
    {
        for (int i = 0; i < 120; i++)  {
            Feld[Stufe][i] = _feld[i];
        }
    }
    // Hash für die Startstellung direkt beim Erstellen berechnen!
    this->hash_wert = zobrist_hash_berechnen(Feld[Stufe], Farbe);
}

string int_array_to_string(int int_array[], int size_of_array) {
    ostringstream oss("");

    for (int temp = 0; temp < size_of_array; temp++)
        oss << int_array[temp];
    return oss.str();
}

string Spielfeld::hash() {
    // Schritt 1: Erzeuge den langen, eindeutigen String, der den Zustand beschreibt.
    // Dies ist der teuerste Schritt, daher rufen wir ihn nur EINMAL auf.
    string kompletterZustand = int_array_to_string(Feld[this->Stufe], 120);
    kompletterZustand += (this->Farbe > 0) ? "w" : "b";

    // Schritt 2: Konvertiere diesen langen String in eine kurze, schnelle 64-bit Zahl.
    // std::hash ist dafür optimiert und sehr schnell.
    std::size_t numerischerHash = std::hash<std::string> {}(kompletterZustand);

    // Schritt 3: Gib diese Zahl als kurzen String zurück.
    // Vergleiche und Kopien dieses kurzen Strings sind extrem schnell.
    return to_string(numerischerHash);
}

inline void Spielfeld::zug_reset()  {
    Stufe = 0;

    for (int i = 0; i < 120; i++)  {
        Feld[Stufe][i] = Feld[Stufe + 1][i];
    }

    for (int j = 0; j < ende; j++)  {
        bester_zug[j].z.pos.pos1 = 0;
        bester_zug[j].z.pos.pos2 = 0;
    }
    find_kings();  // Koenigspositionen nach Brett-Reset aktualisieren
}

feldtyp * Spielfeld::to_feldtyp(feldtyp *neues) {
    {
        for (int i = 0; i < 120; i++)  {
            neues->feld[i] = Feld[Stufe][i];
        }
    }
    return neues;
}

feldtyp * Spielfeld::to_feldtyp() {
    feldtyp *neues = new feldtyp;
    {
        for (int i = 0; i < 120; i++)  {
            neues->feld[i] = Feld[Stufe][i];
        }
    }

    return neues;
}

inline int * Spielfeld::to_feld(int _feld[120])  {
    {
        for (int i = 0; i < 120; i++)  {
            _feld[i] = Feld[Stufe][i];
        }
    }
    return _feld;
}

inline int * Spielfeld::to_feld()  {
    int *_feld = new int [120];
    {
        for (int i = 0; i < 120; i++)  {
            _feld[i] = Feld[Stufe][i];
        }
    }

    return _feld;
}

inline void Spielfeld::copy(Spielfeld& src) {
    this->Farbe = src.Farbe;
    this->Stufe = src.Stufe;
    // Königsposition inkrementell mitführen
    this->wking = src.wking;
    this->bking = src.bking;
    this->hash_wert = src.hash_wert;
}

inline void Spielfeld::setPos(int _feld[], int _farbe, int _stufe, vector<string> & _zuege) {
    setFarbe(_farbe);
    setStufe(_stufe);
    {
        for (int i = 0; i < 120; i++)  {
            Feld[Stufe][i] = _feld[i];
        }
    }
    _zuege.clear();

    //Koenigsposition initial bestimmen
    find_kings();

    // Einmalige Initial-Berechnung für die Grundstellung
    this->hash_wert = zobrist_hash_berechnen(Feld[Stufe], Farbe);

    return;
}

inline void Spielfeld::setFarbe(int _farbe)  {
    Farbe = _farbe;
}

inline void Spielfeld::setStufe(int _stufe)  {
    Stufe = _stufe;
}

inline int Spielfeld::getStufe()  {
    return Stufe;
}

inline void Spielfeld::zug(denkpaar& _zug)  {
    uint64_t neuer_hash = this->hash_wert; // Start mit aktuellem Hash

    setStufe(Stufe + 1);

    // Brett von der vorherigen Stufe kopieren
    memcpy(Feld[Stufe], Feld[Stufe - 1], 120 * sizeof(int));

    int p1 = _zug.z.pos.pos1;
    int p2 = _zug.z.pos.pos2;

    // =========================================================
    // 1. HASH-UPDATE: Alte Figuren auf den betroffenen Feldern entfernen
    // =========================================================
    if (Feld[Stufe][p1] != LEER && Feld[Stufe][p1] != RAND) neuer_hash ^= zobrist_figuren[Feld[Stufe][p1] + 13][p1];
    if (Feld[Stufe][p2] != LEER && Feld[Stufe][p2] != RAND) neuer_hash ^= zobrist_figuren[Feld[Stufe][p2] + 13][p2];
    for (int j = 0; j < _zug.nw; j++) {
        int vp = _zug.verwandelung[j].pos1;
        // Verhindert Doppel-XOR, falls eine Verwandlung auf dem Zielfeld p2 stattfindet
        if (vp != p1 && vp != p2) {
            if (Feld[Stufe][vp] != LEER && Feld[Stufe][vp] != RAND)
                neuer_hash ^= zobrist_figuren[Feld[Stufe][vp] + 13][vp];
        }
    }

    // =========================================================
    // 2. ZUG AUSFÜHREN
    // =========================================================

    // Welche Figur wurde gezogen
    int bewegte_figur = Feld[Stufe][_zug.z.pos.pos1];
    int figur_typ = abs(bewegte_figur);

    //Zug ausfuehren
    Feld[Stufe][_zug.z.pos.pos2] = bewegte_figur;
    Feld[Stufe][_zug.z.pos.pos1] = LEER;

    //Koenigsposition aktualisieren, falls bewegt
    if (figur_typ == W_K || figur_typ == W_Kr) {
        if (bewegte_figur > 0)
            wking = _zug.z.pos.pos2;
        else
            bking = _zug.z.pos.pos2;
    }

    //Verwandlungen ausfuehren (Rochade, Bauernumwandlung, en passant)
    if ((_zug.nw)) {
        int max = _zug.nw;

        for (int j = 0; j < max; j++)    {
            Feld[Stufe][_zug.verwandelung[j].pos1] = _zug.verwandelung[j].fig;

            // Auch bei Rochade wird der Koenigstyp geaendert - Position aktualisieren
            int verwandlung_typ = abs(_zug.verwandelung[j].fig);
            if (verwandlung_typ == W_K || verwandlung_typ == W_Kr) {
                if (_zug.verwandelung[j].fig > 0)
                    wking = _zug.verwandelung[j].pos1;
                else
                    bking = _zug.verwandelung[j].pos1;
            }
        }
    }

    //Seite am Zug wechseln
    setFarbe(Farbe * -1);
    Z = false;

    // Farbwechsel im Hash vermerken
    neuer_hash ^= zobrist_seite;

    // =========================================================
    // 3. HASH-UPDATE: Neue Figuren auf denselben Feldern hinzufügen
    // =========================================================
    if (Feld[Stufe][p1] != LEER && Feld[Stufe][p1] != RAND) neuer_hash ^= zobrist_figuren[Feld[Stufe][p1] + 13][p1];
    if (Feld[Stufe][p2] != LEER && Feld[Stufe][p2] != RAND) neuer_hash ^= zobrist_figuren[Feld[Stufe][p2] + 13][p2];
    for (int j = 0; j < _zug.nw; j++) {
        int vp = _zug.verwandelung[j].pos1;
        if (vp != p1 && vp != p2) {
            if (Feld[Stufe][vp] != LEER && Feld[Stufe][vp] != RAND)
                neuer_hash ^= zobrist_figuren[Feld[Stufe][vp] + 13][vp];
        }
    }

    // Fertig berechneten Hash speichern
    this->hash_wert = neuer_hash;

}

inline void Spielfeld::norm_zug(denkpaar& _zug)  {
    Feld[0][_zug.z.pos.pos2] = Feld[Stufe][_zug.z.pos.pos1];
    Feld[0][_zug.z.pos.pos1] = LEER;

    if ((_zug.nw)) {
        int max = _zug.nw;

        for (int j = 0; j < max; j++)    {
            Feld[0][_zug.verwandelung[j].pos1] = _zug.verwandelung[j].fig;
        }
    }
    setFarbe(Farbe * -1);
    Z = false;


}

inline void Spielfeld::realer_zug(denkpaar& _zug, vector<string>& _zuege)  {
    zug (_zug);
    zuege_append(_zuege, this->hash());
    return;
}

inline void Spielfeld::add_zug(const int & pos1,
                               const int & pos2,
                               const int & _n,
                               const bool& _kill,
                               const int & _figur,
                               const double & _order)  {



    zugstapel[Stufe][_n].z.pos.pos1 = pos1;
    zugstapel[Stufe][_n].z.pos.pos2 = pos2;
    zugstapel[Stufe][_n].kill       = _kill;
    zugstapel[Stufe][_n].figur      = _figur;
    zugstapel[Stufe][_n].order      = _order;
    n++;
    return;
}

inline void Spielfeld::add_verwandelung(const int& farbe,
                                        const int& pos,
                                        const int& was,
                                        const int& n) {
    zugstapel[Stufe][n].verwandelung[zugstapel[Stufe][n].nw].pos1 = pos;
    zugstapel[Stufe][n].verwandelung[zugstapel[Stufe][n].nw].fig  = was * farbe;
    zugstapel[Stufe][n].nw++;
}

inline bool Spielfeld::look_richtung_td(const int feld[], const int &farbe, const int &pos, const int &step)  {
    int zielfeld, farbvorzeichen, i;
    for (i = pos + step; 19 < i && i < 100; i += step)  {
        zielfeld = feld[i];
        if (zielfeld == RAND)
            break;
        if (zielfeld == LEER)
            continue;

        farbvorzeichen = abs(zielfeld) / zielfeld;

        if (farbvorzeichen == farbe)
            break;

        if ((zielfeld == W_D * farbe * -1) ||
                (zielfeld == W_T * farbe * -1) || (zielfeld == W_Tr * farbe * -1))
            return true;
        else
            break;
    }
    return false;
}

inline bool Spielfeld::look_richtung_ld(const int feld[], const int &farbe, const int &pos, const int &step)  {
    int zielfeld, farbvorzeichen, i;
    for (i = pos + step; 19 < i && i < 100; i += step)  {
        zielfeld = feld[i];
        if (zielfeld == RAND)
            break;
        if (zielfeld == LEER)
            continue;

        farbvorzeichen = abs(zielfeld) / zielfeld;

        if (farbvorzeichen == farbe)
            break;

        if ((zielfeld == W_D * farbe * -1) ||
                (zielfeld == W_L * farbe * -1) )
            return true;
        else
            break;
    }
    return false;
}

inline bool Spielfeld::test_drohung(int feld[], int farbe, int pos)  {
    if ((pos < 21) || (pos > 98)) {
        return false;
    }

    return look_richtung_td(feld, farbe, pos, 10) ||
           look_richtung_td(feld, farbe, pos, -10) ||
           look_richtung_td(feld, farbe, pos,  1) ||
           look_richtung_td(feld, farbe, pos, -1) ||
           look_richtung_ld(feld, farbe, pos,  9) ||
           look_richtung_ld(feld, farbe, pos,  -9) ||
           look_richtung_ld(feld, farbe, pos, 11) ||
           look_richtung_ld(feld, farbe, pos, -11) ||

           (feld[pos + 21 * farbe] == W_P * farbe * -1) ||
           (feld[pos + 12 * farbe] == W_P * farbe * -1) ||
           (feld[pos + 19 * farbe] == W_P * farbe * -1) ||
           (feld[pos + 8  * farbe] == W_P * farbe * -1) ||
           (feld[pos + -21 * farbe] == W_P * farbe * -1) ||
           (feld[pos + -12 * farbe] == W_P * farbe * -1) ||
           (feld[pos + -19 * farbe] == W_P * farbe * -1) ||
           (feld[pos + -8  * farbe] == W_P * farbe * -1) ||

           (feld[pos + 9  * farbe] == W_B * farbe * -1) ||
           (feld[pos + 11 * farbe] == W_B * farbe * -1) ||
           (feld[pos + 9  * farbe] == W_Bx * farbe * -1) ||
           (feld[pos + 11 * farbe] == W_Bx * farbe * -1) ||
           (feld[pos + 9  * farbe] == W_Bp_l * farbe * -1) ||
           (feld[pos + 11 * farbe] == W_Bp_l * farbe * -1) ||
           (feld[pos + 9  * farbe] == W_Bp_r * farbe * -1) ||
           (feld[pos + 11 * farbe] == W_Bp_r * farbe * -1) ||

           (feld[pos + 11 * farbe] == W_K * farbe * -1 ||
            feld[pos + -11 * farbe] == W_K * farbe * -1 ||
            feld[pos + 1 * farbe] == W_K * farbe * -1 ||
            feld[pos + -1 * farbe] == W_K * farbe * -1 ||
            feld[pos + 10 * farbe] == W_K * farbe * -1 ||
            feld[pos + -10 * farbe] == W_K * farbe * -1 ||
            feld[pos + 9 * farbe] == W_K * farbe * -1 ||
            feld[pos + -9 * farbe] == W_K * farbe * -1 ||

            feld[pos + 11 * farbe] == W_Kr * farbe * -1 ||
            feld[pos + -11 * farbe] == W_Kr * farbe * -1 ||
            feld[pos + 1 * farbe] == W_Kr * farbe * -1 ||
            feld[pos + -1 * farbe] == W_Kr * farbe * -1 ||
            feld[pos + 10 * farbe] == W_Kr * farbe * -1 ||
            feld[pos + -10 * farbe] == W_Kr * farbe * -1 ||
            feld[pos + 9 * farbe] == W_Kr * farbe * -1 ||
            feld[pos + -9 * farbe] == W_Kr * farbe * -1 );
}

inline void Spielfeld::find_kings()  {
    // setzt wking und bking auf entsprechende Werte

    wking = 0;
    bking = 0;
    int farbvorzeichen;
    int figur;

    for (int i = 21; i <= 98; i++) {

        figur = (abs(Feld[Stufe][i]));

        if ((figur == LEER) || (figur == RAND))
            continue;


        farbvorzeichen = figur / Feld[Stufe][i];

        if ((figur == W_K) || (figur == W_Kr)) {
            if (farbvorzeichen > 0)
                wking = i;
            else
                bking = i;
        }
    }
}

int Spielfeld::zuggenerator()  {
    int pos1, pos2;
    int enp_l, enp_r;
    int ziel, zielfeld;
    int farbvorzeichen, figur;



    int zugnr = 0;

    n = 0; // Variable der Klasse, wenn man es neu deklarieren wuerde,
    // kommt es zu einem seltsamen Fehler in der Zugsortierung,
    // weil n lokal dann 0 bleibt
    int en_passent_bauer = 0;
    spezial = NICHTS;
    test = 0;

    for (int o = 0; o < 199; o++)
        zugstapel[Stufe][o].nw = 0;

    for (int i = 21; i <= 98; i++) {
        figur = (abs(Feld[Stufe][i]));


        if ((figur == LEER) || (figur == RAND))

            continue;

        farbvorzeichen = figur / Feld[Stufe][i];


        pos1           = i;

        if (farbvorzeichen == Farbe)      {
            if ((figur == W_Bx) || (figur == W_B) || (figur == W_Bp_r) ||
                    (figur == W_Bp_l))        {
                // Bauernschlag fuer Startbauer, normalen Bauern, en passant-Bauer
                if ((figur == W_Bp_r) || (figur == W_Bp_l))  {
                    // en passant bauer, der nicht gezogen wird, kann spaeter nicht mehr
                    en_passent_bauer = pos1;
                }




                ziel = pos2 = pos1 + farbvorzeichen * 9;

                // links vor und schlagen
                zielfeld = Feld[Stufe][ziel];

                if  (figur == W_Bp_l)  {
                    add_verwandelung(farbvorzeichen, pos2 - 10 * farbvorzeichen, LEER, n);
                    add_verwandelung(farbvorzeichen, pos2,      W_B,  n);
                    add_zug(pos1, pos2, n, true, figur, 0);
                }


                if (zielfeld != RAND)   {
                    if (zielfeld != LEER)   {
                        // schraeg schlagen
                        if (zielfeld == -1 * W_K * farbvorzeichen || zielfeld == -1 * W_Kr * farbvorzeichen)      {
                            break;
                        }

                        if (zielfeld / abs(zielfeld) != farbvorzeichen)   {
                            spezial = UNRUH;

                            if (figur == W_Bx || figur == W_Bp_l || figur == W_Bp_r) {
                                add_verwandelung(farbvorzeichen, pos2, W_B, n);
                            } else if (figur == W_B)  { // Bauernumwandelung per schlag
                                if (Farbe > 0)  {
                                    if ((91 <= pos2) && (pos2 <= 98))  {
                                        add_verwandelung(farbvorzeichen, pos2, W_D, n);
                                        add_zug(pos1, pos2, n, true, W_D, 900);
                                        add_verwandelung(farbvorzeichen, pos2, W_P, n);
                                        add_zug(pos1, pos2, n, true, W_P, 300);

                                        //		break;
                                    }
                                } else if (Farbe < 0)  {
                                    if ((21 <= pos2) && (pos2 <= 28))  {
                                        add_verwandelung(farbvorzeichen, pos2, W_D, n);
                                        add_zug(pos1, pos2, n, true, W_D, 900);
                                        add_verwandelung(farbvorzeichen, pos2, W_P, n);
                                        add_zug(pos1, pos2, n, true, W_P, 300);

                                        //		break;
                                    }
                                }
                            }
                            add_zug(pos1, pos2, n, true, figur, materialwert[abs(zielfeld)]-materialwert[abs(figur)]*0.1);
                        }
                    }
                }

                // rechts vor
                pos2     = ziel = pos1 + farbvorzeichen * 11;
                zielfeld = Feld[Stufe][ziel];

                if  (figur == W_Bp_r)  {
                    add_verwandelung(farbvorzeichen, pos2 - 10 * farbvorzeichen, LEER, n);
                    add_verwandelung(farbvorzeichen, pos2,      W_B,  n);
                    add_zug(pos1, pos2, n, true, figur, 0);
                } else

                    if (((zielfeld = Feld[Stufe][ziel]) != RAND))   {
                        if (zielfeld != LEER)   { // schraeg schlagen
                            if (zielfeld == -1 * W_K * farbvorzeichen)      {
                                break;
                            }

                            if (zielfeld / abs(zielfeld) != farbvorzeichen)   {
                                spezial = UNRUH;

                                if (figur == W_Bx || figur == W_Bp_l || figur == W_Bp_r)
                                    add_verwandelung(farbvorzeichen, pos2, W_B, n);
                                else if (figur == W_B)  { // Bauernumwandelung per schlag
                                    if (Farbe > 0)  {
                                        if ((91 <= pos2) && (pos2 <= 98))  {
                                            add_verwandelung(farbvorzeichen, pos2, W_D, n);
                                            add_zug(pos1, pos2, n, true, figur, 900);
                                            add_verwandelung(farbvorzeichen, pos2, W_P, n);
                                            add_zug(pos1, pos2, n, true, figur, 300);

                                            //		break;
                                        }
                                    } else if (Farbe < 0)  {
                                        if ((21 <= pos2) && (pos2 <= 28))  {
                                            add_verwandelung(farbvorzeichen, pos2, W_D, n);
                                            add_zug(pos1, pos2, n, true, figur, 900);
                                            add_verwandelung(farbvorzeichen, pos2, W_P, n);
                                            add_zug(pos1, pos2, n, true, figur, 300);

                                            //		break;
                                        }
                                    }
                                }
                                add_zug(pos1, pos2, n, true, figur, materialwert[abs(zielfeld)]-materialwert[abs(figur)]*0.1);
                            }
                        }
                    }
            } //Bauer endet hier

            // andere figuren
            for (int richtung = 0; richtung <= bewegung[figur][0]; richtung++)  {
                for (int weite = 0; weite <= bewegung[figur][1]; weite++)  {
                    pos2 =  pos1 + farbvorzeichen * bewegung[figur][2 + richtung] *
                            (weite + 1);
                    int zielfeld = Feld[Stufe][pos2];

                    // schlagen
                    if (zielfeld != LEER)  {
                        int zielfigur = abs(zielfeld);

                        if (zielfeld == RAND)                       // Aus!
                            break;

                        if (zielfeld / zielfigur == farbvorzeichen) // eigene Figur
                            break;

                        if      ((figur == W_Bx) || (figur == W_Bp_l) || (figur  == W_B) ||
                                 (figur == W_Bp_r)) // kein Bauer schlaegt geradeaus
                            break;



                        if ((zielfeld == -1 * W_K * farbvorzeichen) ||
                                (zielfeld == -1 * W_Kr * farbvorzeichen))        {
                            break;
                        }



                        if ((figur == W_Kr) || (figur == W_K)) {
                            if (test_drohung(Feld[Stufe], Farbe, pos1)) {
                                test = 1;

                            }
                            if (test_drohung(Feld[Stufe], Farbe, pos2))  {
                                test = 1;
                                break;
                            }
                        }

                        if (figur == W_Kr) {
                            add_verwandelung(farbvorzeichen, pos2, W_K, n);
                        } else if (figur == W_Tr) {
                            add_verwandelung(farbvorzeichen, pos2, W_T, n);
                        }



                        spezial = UNRUH;
                        add_zug(pos1, pos2, n, true, figur, materialwert[abs(zielfeld)]-materialwert[abs(figur)]*0.1);

                        break;
                    }

                    // gehen
                    if (figur == W_B)  { // Bauernumwandlung
                        if (Farbe > 0)  {
                            if ((91 <= pos2) && (pos2 <= 98))  {
                                add_verwandelung(farbvorzeichen, pos2, W_D, n);
                                add_zug(pos1, pos2, n, false, figur, 900);
                                add_verwandelung(farbvorzeichen, pos2, W_P, n);
                                add_zug(pos1, pos2, n, false, figur, 300);

                                //		break;
                            }
                        } else if (Farbe < 0)  {
                            if ((21 <= pos2) && (pos2 <= 28))  {
                                add_verwandelung(farbvorzeichen, pos2, W_D, n);
                                add_zug(pos1, pos2, n, false, figur, 900);
                                add_verwandelung(farbvorzeichen, pos2, W_P, n);
                                add_zug(pos1, pos2, n, false, figur, 300);

                                //		break;
                            }
                        }
                    } else

                        if ((figur == W_Bp_r) || (figur == W_Bp_l))
                            add_verwandelung(
                                farbvorzeichen,
                                pos2,
                                W_B,
                                n);
                        else if (figur == W_Bx) {
                            add_verwandelung(farbvorzeichen, pos2, W_B, n);

                            if (weite == 1)  {
                                enp_r = Feld[Stufe][pos2 + farbvorzeichen];
                                enp_l = Feld[Stufe][pos2 - farbvorzeichen];

                                if (enp_r == -1 * W_B * farbvorzeichen)  {
                                    add_verwandelung(farbvorzeichen * -1,
                                                     pos2 + farbvorzeichen,
                                                     W_Bp_r,
                                                     n);
                                }

                                if (enp_l == -1 * W_B * farbvorzeichen)  {
                                    add_verwandelung(farbvorzeichen * -1,
                                                     pos2 - farbvorzeichen,
                                                     W_Bp_l,
                                                     n);
                                }
                            }
                        }

                    if ((figur == W_Kr) || (figur == W_K) ) {
                        if (test_drohung(Feld[Stufe], Farbe, pos1)) {
                            test = 1;

                        }
                        if (test_drohung(Feld[Stufe], Farbe, pos2))  {
                            //    test = 1;
                            break;
                        }

                    }

                    if (figur == W_Kr) {
                        add_verwandelung(farbvorzeichen, pos2, W_K, n);
                    } else if (figur == W_Tr) {
                        add_verwandelung(farbvorzeichen, pos2, W_T, n);
                    }



                    add_zug(pos1, pos2, n, false, figur, 0);
                }
            }
        }
    }

    // Rochade

    if ((Feld[Stufe][25] == W_Kr) || (Feld[Stufe][95] == S_Kr))  {
        if (Farbe < 0) {
            if ((Feld[Stufe][95] == S_Kr) && (Feld[Stufe][96] == LEER) &&
                    (Feld[Stufe][97] == LEER) && (Feld[Stufe][98] == S_Tr))  {
                if (!test_drohung(Feld[Stufe], Farbe,
                                  95) &&
                        !test_drohung(Feld[Stufe], Farbe,
                                      96) && !test_drohung(Feld[Stufe], Farbe, 97))  {
                    add_verwandelung(Farbe, 97, W_K,  n);
                    add_verwandelung(Farbe, 96, W_T,  n);
                    add_verwandelung(Farbe, 98, LEER, n);
                    add_zug(95, 97, n, false, figur, 150);
                }
            }

            if ((Feld[Stufe][95] == S_Kr) && (Feld[Stufe][94] == LEER) &&
                    (Feld[Stufe][93] == LEER) && (Feld[Stufe][92] == LEER) &&
                    (Feld[Stufe][91] == S_Tr))  {
                if (!test_drohung(Feld[Stufe], Farbe, 95) &&
                        !test_drohung(Feld[Stufe], Farbe, 94) && !test_drohung(Feld[Stufe], Farbe, 93))  {
                    add_verwandelung(Farbe, 93, W_K,  n);
                    add_verwandelung(Farbe, 94, W_T,  n);
                    add_verwandelung(Farbe, 91, LEER, n);
                    add_zug(95, 93, n, false, figur, 150);
                }
            }
        } else if (Farbe > 0) {
            if ((Feld[Stufe][25] == W_Kr) && (Feld[Stufe][26] == LEER) &&
                    (Feld[Stufe][27] == LEER) && (Feld[Stufe][28] == W_Tr))  {
                if (!test_drohung(Feld[Stufe], Farbe, 25) &&
                        !test_drohung(Feld[Stufe], Farbe, 26) && !test_drohung(Feld[Stufe], Farbe, 27))  {
                    add_verwandelung(Farbe, 27, W_K,  n);
                    add_verwandelung(Farbe, 26, W_T,  n);
                    add_verwandelung(Farbe, 28, LEER, n);
                    add_zug(25, 27, n, false, figur, 150);
                }
            }

            if ((Feld[Stufe][25] == W_Kr) && (Feld[Stufe][24] == LEER) &&
                    (Feld[Stufe][23] == LEER) && (Feld[Stufe][22] == LEER) &&
                    (Feld[Stufe][21] == W_Tr))  {
                if (!test_drohung(Feld[Stufe], Farbe, 25) &&
                        !test_drohung(Feld[Stufe], Farbe, 24) && !test_drohung(Feld[Stufe], Farbe, 23))  {
                    add_verwandelung(Farbe, 23, W_K,  n);
                    add_verwandelung(Farbe, 24, W_T,  n);
                    add_verwandelung(Farbe, 21, LEER, n);
                    add_zug(25, 23, n, false, figur, 150);
                }
            }
        }
    }

    // wenn ein en passent bauer gefunden wurde, dann musser bei allen zuegen,
    // ausser bei seinen eigenen im naechsten zug umgewandelt werden
    if (en_passent_bauer != 0)  {
        int q;

        for (q = 0; q < n; q++)  {
            figur = zugstapel[Stufe][q].figur;

            if (!((figur  == W_Bp_l) || (figur  == W_Bp_r)))  {
                add_verwandelung(Farbe, en_passent_bauer, W_B, q);
            }
        }
    }

    return n;
}

inline denkpaar * Spielfeld::makeZugstapel()  {
    zuggenerator();
    Z = true;
    return zugstapel[Stufe];
}

int schach_bewegung[15][15] = { // Richtung, Weite, wohin[richtung]
    { 0, 0, 0   },              // NIL!
    { 7, 0, 21, -21, 12, -12, 19, -19, 8, -8},               // Pferd
    { 7, 6, 1, -1, 9, -9, 10, -10, 11, -11},              // Dame;
};

inline bool Spielfeld::schach(int _farbe)  {
    /*	int pos1, pos2, figur, farbvorzeichen;


            for (int i=21; i<=98; i++ )	{  //König finden
                    figur= (abs(Feld[Stufe][i]));
                    if (figur == W_K || figur == W_Kr)  {
                            farbvorzeichen = figur/Feld[Stufe][pos1];
                            if (farbvorzeichen == _farbe)  {
                                    pos1 = i;
                                    break;
                                    }
                            }
                    }
            for (int schach_bewegung_figur = 1; schach_bewegung_figur <= 2;
               schach_bewegung_figur++)  {
                    for (int richtung = 0; richtung
                       <=schach_bewegung[schach_bewegung_figur][0]; richtung++)  {
                            for (int weite = 0; weite <=
                               schach_bewegung[schach_bewegung_figur][1]; weite++)
                                {
                                    pos2 =	pos1 + farbvorzeichen *
                                       schach_bewegung[schach_bewegung_figur][2+richtung]
                                       * (weite+1);
                                    int pos2_figur = Feld[Stufe][pos2];
                                    if (zielfeld != LEER)  {
                                            int zielfigur = abs(zielfeld);
                                            if (zielfeld == RAND) // über den
                                               Tellerrand schauen...
                                                    break;
                                            if (zielfeld/zielfigur ==
                                               farbvorzeichen)   // eigene Figur
                                                    break;
                                            if    ((figur == W_Bx) || (figur ==
                                               W_Bp_l) || (figur  == W_B) ||
                                               (figur == W_Bp_r))	// kein Bauer
                                               schlÃ¤gt geradeaus
                                                    break;
                                            return 0;
                                            }
                                            add_zug(pos1, pos2, n, true);
                                    break;
                                    }
                            add_zug(pos1, pos2, n);
                                    }
                            }
                    }
            return n;*/
    return 0;
}

void Spielfeld::disp()  {
    cout << "\n";
    int breite = 3;

    cout << "      " << "  >--A--v--B--v--C--v--D--v--E--v--F--v--G--v--H--<\n";
    cout << "      " << "v >-----+-----+-----+-----+-----+-----+-----+-----< v\n" <<
         "      ";

    for (int j = 9; j > 1; j--)  {
        cout << j - 1;

        for (int i = 1; i < 9; i++)  {
            if (Feld[Stufe][j * 10 + i] != RAND)
                cout << setw(breite) << " | " << setw(
                         breite) << figuren_char[Feld[Stufe][j * 10 + i] + figurenanzahl];
        }
        cout << setw(breite) << "| " << j - 1 << "\n" << "      " <<
             "^ >-----+-----+-----+-----+-----+-----+-----+-----< ^\n" << "      ";
    }
    cout << "  >--A--+--B--+--C--+--D--+--E--+--F--+--G--+--H--< ^\n";
    cout << "\n";
}

void Spielfeld::disp_cleanest()  {
    cout << "\n";

    for (int j = 9; j > 1; j--)  {
        for (int i = 1; i < 9; i++)  {
            if (Feld[Stufe][j * 10 + i] !=
                    RAND)
                cout << figuren_char[Feld[Stufe][j * 10 + i] + figurenanzahl];

            if (j * 10 + i != 28)
                cout << ",";
        }
        cout << "\n";
    }
    cout << "\n";
}

void Spielfeld::print_zugstapel()  {
    for (int i = 0; i < n; i++)  {
        cout << figuren_char[zugstapel[Stufe][i].figur  + figurenanzahl] << ": "
             << int(zugstapel[Stufe][i].z.pos.pos1) << "(" <<
             grundfeld_bezeichnungen[zugstapel[Stufe][i].z.pos.pos1] << ")  => "
             << int(zugstapel[Stufe][i].z.pos.pos2) << "(" <<
             grundfeld_bezeichnungen[zugstapel[Stufe][i].z.pos.pos2] << ")";

        if (zugstapel[Stufe][i].nw) {
            cout << " | ";
            int max = zugstapel[Stufe][i].nw;

            for (int j = 0; j < max; j++)    {
                cout << int(zugstapel[Stufe][i].verwandelung[j].pos1)
                     << "(" <<
                     grundfeld_bezeichnungen[zugstapel[Stufe][i].verwandelung[j].pos1] <<
                     ")  <= "
                     << figuren_char[zugstapel[Stufe][i].verwandelung[j].fig  +
                                                                              figurenanzahl];

                if (j < max - 1)
                    cout << ", ";
            }
        }

        cout << "\n";
    }
}

void Spielfeld::write()  {
    ofstream partie("partie.txt", ios::app);

    partie << "\n{\n";
    partie << "RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,\n"
           << "RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,\n";

    for (int j = 2; j < 10; j++)  {
        partie << "RAND, ";

        for (int i = 1; i < 9; i++)  {
            if (Feld[Stufe][j * 10 + i] != RAND)
                partie
                        << setw(6)
                        << figuren_intern[Feld[Stufe][j * 10 + i] + figurenanzahl]
                        << ", ";
        }
        partie << "  RAND,";
        partie << "\n";
    }
    partie << "RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,\n"
           << "RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND,	RAND\n";
    partie << "};";
    partie << "\n";
    partie.close();
}

inline double entwicklung(int feld[120], int farbe)    {
    double wertung = 0;
    int    dieses_feld;

    const double ROCHADE_BONUS = 40;
    double Phasen_Faktor = 1.0;

    if (figurenwert < 6000) {
        Phasen_Faktor = figurenwert / 6000;
    }

    //double faktor_eigene_farbe = 1;
    //if (farbe != eigene_farbe) faktor_eigene_farbe = 2;

    for (int i = 21; i <= 98; i++)  {
        //dieses_feld = feld[i];

        if (feld[i] == RAND || feld[i] == LEER)
            continue;

        if (feld[i] == W_Bx || feld[i] == W_B)  {
            wertung += __STARTPUNKTE[i];
            // ************************************************************
            // ** ISOLANI WEISS **
            // ************************************************************
            if ((feld[i-1] != W_B && feld[i-1] != W_Bx) && (feld[i+1] != W_B && feld[i+1] != W_Bx) && (feld[i-11] != W_B && feld[i-11] != W_Bx) && (feld[i-9] != W_B && feld[i-9] != W_Bx) && (feld[i-21] != W_B && feld[i-21] != W_Bx) && (feld[i-19] != W_B && feld[i-19] != W_Bx))
                wertung -= IsolaniScore;
            // ************************************************************
            // ** FREIBAUER WEISS **
            // ************************************************************
            if (i > 50 && feld[i+10] != S_B && feld[i+10] != S_Bx && feld[i+20] != S_B && feld[i+20] != S_Bx && feld[i+30] != S_Bx && feld[i+9] != S_B && feld[i+9] != S_Bx && feld[i+19] != S_B && feld[i+19] != S_Bx && feld[i+29] != S_Bx && feld[i+11] != S_B && feld[i+11] != S_Bx && feld[i+21] != S_B && feld[i+21] != S_Bx && feld[i+31] != S_Bx)
                wertung += FreibauerScore;
        }       // -kingzone_ich[i]*10;	//4.1

        if (feld[i] == S_Bx || feld[i] == S_B) {
            wertung -= __STARTPUNKTE[119 - i];
            // ************************************************************
            // ** ISOLANI SCHWARZ **
            // ************************************************************
            if ((feld[i-1] != S_B && feld[i-1] != S_Bx) && (feld[i+1] != S_B && feld[i+1] != S_Bx) && (feld[i+11] != S_B && feld[i+11] != S_Bx) && (feld[i+9] != S_B && feld[i+9] != S_Bx) && (feld[i+21] != S_B && feld[i+21] != S_Bx) && (feld[i+19] != S_B && feld[i+19] != S_Bx))
                wertung += IsolaniScore;
            // ************************************************************
            // ** FREIBAUER SCHWARZ **
            // ************************************************************
            if (i < 60 && feld[i-10] != W_B && feld[i-10] != W_Bx && feld[i-20] != W_B && feld[i-20] != W_Bx && feld[i-30] != W_Bx && feld[i-9] != W_B && feld[i-9] != W_Bx && feld[i-19] != W_B && feld[i-19] != W_Bx && feld[i-29] != W_Bx && feld[i-11] != W_B && feld[i-11] != W_Bx && feld[i-21] != W_B && feld[i-21] != W_Bx && feld[i-31] != W_Bx)
                wertung -= FreibauerScore;
        }
        if (feld[i] == W_P) {
            wertung += __STARTPUNKTEx2[i];   // +kingzone_ich[i]*10;	//1.17
        }
        if (feld[i] == S_P) {
            wertung -= __STARTPUNKTEx2[119 - i];   // -kingzone_gegner[i]*10;
        }
        if (feld[i] == W_L) {
            wertung += __STARTPUNKTEx4[i];
        }
        if (feld[i] == S_L) {
            wertung -= __STARTPUNKTEx4[119 - i];
        }
        if (feld[i] == W_T || feld[i] == W_Tr) {
            wertung += __STARTPUNKTEx6[i];
        }
        if (feld[i] == S_T || feld[i] == S_Tr) {
            wertung -= __STARTPUNKTEx6[119 - i];
        }
        if (feld[i] == W_D) {
            wertung += __STARTPUNKTEx8[i];
        }
        if (feld[i] == S_D) {
            wertung -= __STARTPUNKTEx8[119 - i];
        }
        if (feld[i] == W_K || feld[i] == W_Kr) {
            double pawn_shield_score_w = 0;
            wertung +=  (figurenwert-4100)*0.0005*__STARTPUNKTEx10[i];
            if (feld[i] == W_Kr) {
                if (feld[21] == W_Tr || feld[28] == W_Tr) {
                    wertung += ROCHADE_BONUS * Phasen_Faktor;
                    if (feld[21] == W_Tr) { //lange Rochade möglich
                        if (feld[31] != W_Bx)
                            wertung -= 15 * Phasen_Faktor;
                        if (feld[32] != W_Bx)
                            wertung -= 20 * Phasen_Faktor;
                        if (feld[33] != W_Bx)
                            wertung -= 30 * Phasen_Faktor;
                    }
                    if (feld[28] == W_Tr) { //kurze Rochade möglich
                        if (feld[38] != W_Bx)
                            wertung -= 15 * Phasen_Faktor;
                        if (feld[37] != W_Bx)
                            wertung -= 20 * Phasen_Faktor;
                        if (feld[36] != W_Bx)
                            wertung -= 30 * Phasen_Faktor;
                    }
                }
            }

            if (i == 27) {
                if (feld[36] == W_Bx)
                    pawn_shield_score_w += 12;
                else
                    pawn_shield_score_w -= 10;
                if (feld[37] == W_Bx)
                    pawn_shield_score_w += 20;
                else
                    pawn_shield_score_w -= 25;
                if (feld[38] == W_Bx)
                    pawn_shield_score_w += 10;
                else
                    pawn_shield_score_w -= 7;
            }
            if (i == 22 || i == 23) {
                if (feld[31] == W_Bx)
                    pawn_shield_score_w += 10;
                else
                    pawn_shield_score_w -= 7;
                if (feld[32] == W_Bx)
                    pawn_shield_score_w += 20;
                else
                    pawn_shield_score_w -= 25;
                if (feld[33] == W_Bx)
                    pawn_shield_score_w += 12;
                else
                    pawn_shield_score_w -= 10;
            }
            wertung += pawn_shield_score_w * Phasen_Faktor;
        }
        if (feld[i] == S_K || feld[i] == S_Kr) {
            double pawn_shield_score_b = 0;
            wertung -=  (figurenwert-4100)*0.0005*__STARTPUNKTEx10[119 - i];

            if (feld[i] == S_Kr) {
                if (feld[91] == S_Tr || feld[98] == S_Tr) {
                    wertung -= ROCHADE_BONUS * Phasen_Faktor;
                    if (feld[91] == S_Tr) { //lange Rochade möglich
                        if (feld[81] != S_Bx)
                            wertung += 8 * Phasen_Faktor;
                        if (feld[82] != S_Bx)
                            wertung += 10 * Phasen_Faktor;
                        if (feld[83] != S_Bx)
                            wertung += 15 * Phasen_Faktor;
                    }
                    if (feld[98] == S_Tr) { //kurze Rochade möglich
                        if (feld[88] != S_Bx)
                            wertung += 8 * Phasen_Faktor;
                        if (feld[87] != S_Bx)
                            wertung += 10 * Phasen_Faktor;
                        if (feld[86] != S_Bx)
                            wertung += 15 * Phasen_Faktor;
                    }
                }
            }
            if (i == 97) {
                if (feld[86] == S_Bx)
                    pawn_shield_score_b += 12;
                else
                    pawn_shield_score_b -= 10;
                if (feld[87] == S_Bx)
                    pawn_shield_score_b += 20;
                else
                    pawn_shield_score_b -= 25;
                if (feld[88] == S_Bx)
                    pawn_shield_score_b += 10;
                else
                    pawn_shield_score_b -= 7;
            }
            if (i == 92 || i == 93) {
                if (feld[81] == S_Bx)
                    pawn_shield_score_b += 10;
                else
                    pawn_shield_score_b -= 7;
                if (feld[82] == S_Bx)
                    pawn_shield_score_b += 20;
                else
                    pawn_shield_score_b -= 25;
                if (feld[83] == S_Bx)
                    pawn_shield_score_b += 12;
                else
                    pawn_shield_score_b -= 10;
            }
            wertung -= pawn_shield_score_b * Phasen_Faktor;
        }
    }
    return wertung;
}

inline double material(int feld[120], int farbe)  {
    double wert = 0;

    int    figur;
    int Marker_L_w = 0;
    int Marker_L_s = 0;
    figurenwert = 0;

    for(int j=21; j<99; j++) {
        kingzone[j] = 0;
    }
    for (int i = 21; i <= 98; i++)     {
        figur = feld[i];

        if ((figur == RAND) || (figur == LEER))
            continue;

        if (figur == W_L) {
            wert += figur * materialwert[W_L] + Marker_L_w;
            Marker_L_w = 40;
        } else if (figur == -W_L) {
            wert += figur * materialwert[W_L] + Marker_L_s;
            Marker_L_s = -40;
        } else {
            wert += figur * materialwert[abs(figur)];
        }

        if (abs(figur) != W_K && abs(figur) != W_Kr) {
            figurenwert += abs(figur) * materialwert[abs(figur)];
        } else {
            int Vorzeichen = figur/abs(figur);
            kingzone[i-9] = Vorzeichen;
            kingzone[i-10] = Vorzeichen;
            kingzone[i-11] = Vorzeichen;
            kingzone[i-1] = Vorzeichen;
            kingzone[i] = Vorzeichen;
            kingzone[i+1] = Vorzeichen;
            kingzone[i+9] = Vorzeichen;
            kingzone[i+10] = Vorzeichen;
            kingzone[i+11] = Vorzeichen;
        }
    }
    return wert;
}


inline int zuganzahl(int feld[120], int _eigene_farbe)  {
    int pos2;
    int figur;
    int farbvorzeichen;
    int Attack = 0;

    static double schlagzone_ich[120];
    static double schlagzone_gegner[120];

    for(int j=21; j<99; ++j) {
        schlagzone_ich[j] = 0;
        schlagzone_gegner[j] = 0;
        zugzone_ich[j] = 0;
        zugzone_du[j] = 0;
    }

    double n = 0;
    int Anzahl_Angreifer_w = 0;
    int Anzahl_Angreifer_s = 0;
    double Angreifer_Wert_w = 0;
    double Angreifer_Wert_s = 0;

    double K_Safety_Wert = 0;

    for (int i = 21; i <= 98; i++)    {
        if ((i % 10) == 0 || (i % 10) == 9)
            continue; // Randspalten überspringen
        int C_flag = 0;
        figur = abs(feld[i]);

        if (feld[i] > 0) {
            farbvorzeichen = +1;
        } else {
            farbvorzeichen = -1;
        }

        if ((figur == LEER) || (figur == RAND))
            continue;

        if ((figur == W_D)) {
            int Attack_Dame = 0;
            int FS_Dame = 0;

            for (int richtung = 0; richtung <= bewegung[figur][0]; richtung++)  {
                for (int weite = 0; weite <= bewegung[figur][1]; weite++)  {
                    pos2 = i + farbvorzeichen * bewegung[figur][2 + richtung] * (weite + 1);
                    int zielfeld = feld[pos2];

                    /*    for (int richtung = 0; richtung <= bewegung[W_K][0]; richtung++)
                        {
                       for (int weite = 0; weite <= bewegung[W_K][1]; weite++)  {
                       pos2 = i + farbvorzeichen * bewegung[W_K][2+richtung] * (weite+1);
                       int zielfeldk = feld[pos2];
                       if (zielfeld == zielfeldk) n -= 400*farbvorzeichen;return n;}}*/
                    if (zielfeld == RAND) // Aus!
                        break;

                    if (farbvorzeichen == _eigene_farbe)
                        zugzone_ich[pos2] += 10;
                    else
                        zugzone_du[pos2] += 10;
                    if (kingzone[pos2] == -farbvorzeichen) {
                        double angriffs_multiplikator = 1.0;
                        if (farbvorzeichen == _eigene_farbe) {
                            if (farbvorzeichen == 1) {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_w += KSafety * angriffs_multiplikator;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_w += 1;
                            } else {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_s += KSafety * angriffs_multiplikator;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_s += 1;
                            };
                            C_flag = 1;
                        } else {
                            if (farbvorzeichen == 1) {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_w += KSafety * angriffs_multiplikator * 0.5;

                                if (C_flag == 0)
                                    Anzahl_Angreifer_w += 1;
                            } else {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_s += KSafety * angriffs_multiplikator * 0.5;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_s += 1;
                            };
                            C_flag = 1;
                        }
                    }


                    if (zielfeld != LEER)  {
                        int zielfigur = abs(zielfeld);


                        /*   if (abs(zielfeld) == W_K)
                                break;*/

                        if (farbvorzeichen != _eigene_farbe)  {
                            if (zielfeld / _eigene_farbe > 0) {                // Gegner greift meine Figur an
                                if (schlagzone_gegner[pos2] != 1)
                                    schlagzone_gegner[pos2] = 1;
                                else
                                    Attack_Dame += KooIch;
                                //    Attack_Dame += kingzone_ich[pos2] * Koenigsangriff_Er;

                                Attack_Dame += (abs(zielfeld) * materialwert[abs(zielfeld)] - 40) * AttackIch;



                            } else {
                                Attack_Dame += DefIch1;

                                if (abs(zielfeld) < 6)
                                    Attack_Dame -= DefIch2;
                            } // Gegner deckt seine Figuren

                        } else  {
                            if (zielfeld / _eigene_farbe < 0) {                     // Ich greife Gegner an
                                if (schlagzone_ich[pos2] != 1)
                                    schlagzone_ich[pos2] = 1;
                                else
                                    Attack_Dame += KooEr;
                                //  Attack_Dame += kingzone_gegner[pos2] * Koenigsangriff_Ich;

                                if (zielfigur == W_K || zielfigur == W_Kr)
                                    break;
                                Attack_Dame += (abs(zielfeld) * materialwert[abs(zielfeld)]) / AttackEr;
                            } else {
                                Attack_Dame += DefEr1;

                                if (abs(zielfeld) < 6)
                                    Attack_Dame -= DefEr2;
                            }
                        } // Ich decke meine Figuren
                        break;
                    }

                }
            }
            //Wie sicher steht meine Dame?
            if ((feld[i+31*farbvorzeichen] == -W_Bx*farbvorzeichen && feld[i+11*farbvorzeichen]==LEER && feld[i+21*farbvorzeichen]==LEER) || (feld[i+29*farbvorzeichen] == -W_Bx*farbvorzeichen && feld[i+19*farbvorzeichen]==LEER && feld[i+29*farbvorzeichen]==LEER) || ((feld[i+21*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+21*farbvorzeichen] == -W_Bx*farbvorzeichen)&&feld[i+11*farbvorzeichen]==LEER) || ((feld[i+19*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+19*farbvorzeichen] == -W_Bx*farbvorzeichen) && feld[i+9*farbvorzeichen]==LEER) || feld[i+18*farbvorzeichen] == -W_L*farbvorzeichen || feld[i+22*farbvorzeichen] == -W_L*farbvorzeichen)
                FS_Dame -= Figurensicherheit;


            Attack_Dame *= farbvorzeichen;
            FS_Dame *= farbvorzeichen;
            //  cout << MobDame * n_Dame + AttDame * Attack_Dame << "\n";
            n           += AttDame * Attack_Dame + FS_Dame;
        }

        if ((figur == W_T) || (figur == W_Tr)) {
            int n_Turm      = -10;
            int Attack_Turm = 0;
            int FS_Turm = 0;

            for (int richtung = 0; richtung <= bewegung[figur][0]; richtung++)  {
                for (int weite = 0; weite <= bewegung[figur][1]; weite++)  {
                    pos2 = i + farbvorzeichen * bewegung[figur][2 + richtung] * (weite + 1);
                    int zielfeld = feld[pos2];
                    if (zielfeld == RAND) // Aus!
                        break;
                    /*    for (int richtung = 0; richtung <= bewegung[W_K][0]; richtung++)
                        {
                       for (int weite = 0; weite <= bewegung[W_K][1]; weite++)  {
                       pos2 = i + farbvorzeichen * bewegung[W_K][2+richtung] * (weite+1);
                       int zielfeldk = feld[pos2];
                       if (zielfeld == zielfeldk) n -= 400*farbvorzeichen;return n;}}*/
                    if (farbvorzeichen == _eigene_farbe)
                        zugzone_ich[pos2] += 100;
                    else
                        zugzone_du[pos2] += 100;
                    if (kingzone[pos2] == -farbvorzeichen) {
                        double angriffs_multiplikator = 1.0;
                        if (farbvorzeichen == _eigene_farbe) {
                            if (farbvorzeichen == 1) {

                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_w += KSafety * K_Angriff_Turm * angriffs_multiplikator;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_w += 1;
                            } else {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_s += KSafety * K_Angriff_Turm * angriffs_multiplikator;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_s += 1;
                            };
                            C_flag = 1;
                        } else {
                            if (farbvorzeichen == 1) {

                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_w += KSafety * K_Angriff_Turm * angriffs_multiplikator * 0.5;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_w += 1;
                            } else {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_s += KSafety * K_Angriff_Turm * angriffs_multiplikator * 0.5;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_s += 1;
                            };
                            C_flag = 1;
                        }
                    }


                    //     if (kingzone[i] == 1) Attack_Turm += KSafety;
                    if (zielfeld != LEER)  {
                        int zielfigur = abs(zielfeld);



                        /*   if (abs(zielfeld) == W_K)
                                break;*/

                        if (farbvorzeichen != _eigene_farbe)  {
                            if (zielfeld / _eigene_farbe > 0) { // Gegner greift meine Figur
                                // an
                                if (schlagzone_gegner[pos2] != 1)
                                    schlagzone_gegner[pos2] = 1;
                                else
                                    Attack_Turm += KooIch;

                                //         Attack_Turm += kingzone_ich[pos2]/* * Koenigsangriff_Er*/;
                                n_Turm += 3;

                                if ((n_Turm > 6) && (n_Turm < 11))
                                    n_Turm += 2;

                                if ((n_Turm > 10) && (n_Turm < 14))
                                    n_Turm += 1;

                                Attack_Turm +=
                                    (abs(zielfeld) * materialwert[abs(zielfeld)] - 40) * AttackIch;
                            } else {
                                Attack_Turm += DefIch1;

                                if (abs(zielfeld) < 6)
                                    Attack_Turm -= DefIch2;
                            } // Gegner deckt seine Figuren

                        } else  {
                            if (zielfeld / _eigene_farbe < 0) {                     // Ich greife Gegner an
                                if (schlagzone_ich[pos2] != 1)
                                    schlagzone_ich[pos2] = 1;
                                else
                                    Attack_Turm += KooEr;
                                //   Attack_Turm += kingzone_gegner[pos2] * Koenigsangriff_Ich;
                                if (n_Turm < 7)
                                    n_Turm += 3;

                                if ((n_Turm > 6) && (n_Turm < 11))
                                    n_Turm += 2;

                                if ((n_Turm > 10) && (n_Turm < 14))
                                    n_Turm += 1;

                                if (zielfigur == W_K || zielfigur == W_Kr)
                                    break;
                                Attack_Turm += (abs(zielfeld) * materialwert[abs(zielfeld)]) /  AttackEr;
                            } else  {
                                Attack_Turm += DefEr1;

                                if (abs(zielfeld) < 6)
                                    Attack_Turm -= DefEr2;
                            }
                        } // Ich decke meine Figuren
                        break;
                    }

                    if (farbvorzeichen != _eigene_farbe)  {
                        //        Attack_Turm += kingzone_ich[pos2]/* * Koenigsangriff_Er*/;

                        if (n_Turm < 7)
                            n_Turm += 3;

                        if ((n_Turm > 6) && (n_Turm < 11))
                            n_Turm += 2;

                        if ((n_Turm > 10) && (n_Turm < 14))
                            n_Turm += 1;
                    } else  {
                        //  Attack_Turm += kingzone_gegner[pos2] * Koenigsangriff_Ich;
                        if (n_Turm < 7)
                            n_Turm += 3;

                        if ((n_Turm > 6) && (n_Turm < 11))
                            n_Turm += 2;

                        if ((n_Turm > 10) && (n_Turm < 14))
                            n_Turm += 1;
                    }
                }
            }
            //Wie sicher steht mein Turm?
            if ((((feld[i+31*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+11*farbvorzeichen]==LEER)&&(feld[i+21*farbvorzeichen]==LEER)||(feld[i+29*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+19*farbvorzeichen]==LEER)&&(feld[i+29*farbvorzeichen]==LEER)||feld[i+21*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+21*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+11*farbvorzeichen]==LEER))||((feld[i+19*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+19*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+9*farbvorzeichen]==LEER)))
                FS_Turm -= Figurensicherheit;


            n_Turm *= farbvorzeichen;
            Attack_Turm *= farbvorzeichen;
            FS_Turm *= farbvorzeichen;
            n        += MobTurm * n_Turm + AttTurm * Attack_Turm + FS_Turm;
        }

        if ((figur == W_L)) {
            int n_Laeufer  = -15;
            int Attack_Laeufer = 0;
            int FS_Laeufer = 0;

            for (int richtung = 0; richtung <= bewegung[figur][0]; richtung++)  {
                for (int weite = 0; weite <= bewegung[figur][1]; weite++)  {
                    pos2 = i + farbvorzeichen * bewegung[figur][2 + richtung] * (weite + 1);
                    int zielfeld = feld[pos2];
                    if (zielfeld == RAND) // Aus!
                        break;
                    /*    for (int richtung = 0; richtung <= bewegung[W_K][0]; richtung++)
                        {
                       for (int weite = 0; weite <= bewegung[W_K][1]; weite++)  {
                       pos2 = i + farbvorzeichen * bewegung[W_K][2+richtung] * (weite+1);
                       int zielfeldk = feld[pos2];
                       if (zielfeld == zielfeldk) n -= 400*farbvorzeichen;return n;}}*/
                    if (farbvorzeichen == _eigene_farbe)
                        zugzone_ich[pos2] += 1000;
                    else
                        zugzone_du[pos2] += 1000;
                    if (kingzone[pos2] == -farbvorzeichen) {
                        double angriffs_multiplikator = 1.0;
                        if (farbvorzeichen == _eigene_farbe) {
                            if (farbvorzeichen == 1) {

                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_w += KSafety * K_Angriff_Laeufer * angriffs_multiplikator;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_w += 1;
                            } else {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_s += KSafety * K_Angriff_Laeufer * angriffs_multiplikator;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_s += 1;
                            };
                            C_flag = 1;
                        } else {
                            if (farbvorzeichen == 1) {

                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_w += KSafety * K_Angriff_Laeufer * angriffs_multiplikator * 0.5;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_w += 1;
                            } else {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_s += KSafety * K_Angriff_Laeufer * angriffs_multiplikator * 0.5;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_s += 1;
                            };
                            C_flag = 1;
                        }
                    }

                    //     if (kingzone[i] == 1) Attack_Laeufer += KSafety;
                    if (zielfeld != LEER)  {
                        int zielfigur = abs(zielfeld);

                        /*   if (abs(zielfeld) == W_K)
                            break;*/

                        if (farbvorzeichen != _eigene_farbe)  {
                            if (zielfeld / _eigene_farbe > 0) { // Gegner greift meine Figur
                                // an
                                if (schlagzone_gegner[pos2] != 1)
                                    schlagzone_gegner[pos2] = 1;
                                else
                                    Attack_Laeufer += KooIch;
                                //           Attack_Laeufer += kingzone_ich[pos2]/* * Koenigsangriff_Er*/;
                                if (n_Laeufer == -15)
                                    n_Laeufer += 5;  // 12

                                if ((n_Laeufer > -11) && (n_Laeufer < 10))
                                    n_Laeufer += 4;

                                if ((n_Laeufer > 9) && (n_Laeufer < 16))
                                    n_Laeufer += 3;

                                if ((n_Laeufer > 15) && (n_Laeufer < 22))
                                    n_Laeufer += 2;

                                if (n_Laeufer > 21)
                                    n_Laeufer += 1;

                                Attack_Laeufer += (abs(zielfeld) * materialwert[abs(zielfeld)] - 40) * AttackIch;
                            } else {
                                Attack_Laeufer += DefIch1;

                                if (abs(zielfeld) < 6)
                                    Attack_Laeufer -= DefIch2;
                            } // Gegner deckt seine Figuren
                        } else  {
                            if (zielfeld / _eigene_farbe < 0) {  // Ich greife Gegner an
                                if (schlagzone_ich[pos2] != 1)
                                    schlagzone_ich[pos2] = 1;
                                else
                                    Attack_Laeufer += KooEr;
                                //      Attack_Laeufer += kingzone_gegner[pos2] * Koenigsangriff_Ich;
                                if (n_Laeufer == -15)
                                    n_Laeufer += 5;  // 12

                                if ((n_Laeufer > -11) && (n_Laeufer < 10))
                                    n_Laeufer += 4;

                                if ((n_Laeufer > 9) && (n_Laeufer < 16))
                                    n_Laeufer += 3;

                                if ((n_Laeufer > 15) && (n_Laeufer < 22))
                                    n_Laeufer += 2;

                                if (n_Laeufer > 21)
                                    n_Laeufer += 1;

                                if (zielfigur == W_K || zielfigur == W_Kr)
                                    break;
                                Attack_Laeufer += (abs(zielfeld) * materialwert[abs(zielfeld)]) /  AttackEr;
                            } else  { // Ich decke meine Figuren
                                Attack_Laeufer += DefEr1;

                                if (abs(zielfeld) < 6)
                                    Attack_Laeufer -= DefEr2;
                            }
                        }
                        break;
                    }

                    if (farbvorzeichen != _eigene_farbe)  {
                        //               Attack_Laeufer += kingzone_ich[pos2]/* * Koenigsangriff_Er*/;

                        if (n_Laeufer == -15)
                            n_Laeufer += 5;  // 12

                        if ((n_Laeufer > -11) && (n_Laeufer < 10))
                            n_Laeufer += 4;

                        if ((n_Laeufer > 9) && (n_Laeufer < 16))
                            n_Laeufer += 3;

                        if ((n_Laeufer > 15) && (n_Laeufer < 22))
                            n_Laeufer += 2;

                        if (n_Laeufer > 21)
                            n_Laeufer += 1;
                    } else  {
                        //   Attack_Laeufer += kingzone_gegner[pos2] * Koenigsangriff_Ich;
                        if (n_Laeufer == -15)
                            n_Laeufer += 5;  // 12

                        if ((n_Laeufer > -11) && (n_Laeufer < 10))
                            n_Laeufer += 4;

                        if ((n_Laeufer > 9) && (n_Laeufer < 16))
                            n_Laeufer += 3;

                        if ((n_Laeufer > 15) && (n_Laeufer < 22))
                            n_Laeufer += 2;

                        if (n_Laeufer > 21)
                            n_Laeufer += 1;
                    }
                }
            }
            if ((((feld[i+31*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+11*farbvorzeichen]==LEER)&&(feld[i+21*farbvorzeichen]==LEER)||(feld[i+29*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+19*farbvorzeichen]==LEER)&&(feld[i+29*farbvorzeichen]==LEER)||feld[i+21*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+21*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+11*farbvorzeichen]==LEER))||((feld[i+19*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+19*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+9*farbvorzeichen]==LEER)))
                FS_Laeufer -= Figurensicherheit;

            n_Laeufer *= farbvorzeichen;
            Attack_Laeufer *= farbvorzeichen;
            FS_Laeufer *= farbvorzeichen;

            n += MobLau * n_Laeufer + AttLau * Attack_Laeufer + FS_Laeufer;
        }

        if ((figur == W_P)) {
            int Attack_Pferd = 0;
            int FS_Pferd = 0;

            for (int richtung = 0; richtung <= bewegung[figur][0]; richtung++)  {
                for (int weite = 0; weite <= bewegung[figur][1]; weite++)  {
                    pos2 = i + farbvorzeichen * bewegung[figur][2 + richtung] * (weite + 1);
                    int zielfeld = feld[pos2];
                    if (zielfeld == RAND) // Aus!
                        break;
                    /*    for (int richtung = 0; richtung <= bewegung[W_K][0]; richtung++)
                        {
                       for (int weite = 0; weite <= bewegung[W_K][1]; weite++)  {
                       pos2 = i + farbvorzeichen * bewegung[W_K][2+richtung] * (weite+1);
                       int zielfeldk = feld[pos2];
                       if (zielfeld == zielfeldk) n -= 400*farbvorzeichen;return n;}}*/
                    if (farbvorzeichen == _eigene_farbe)
                        zugzone_ich[pos2] += 1000;
                    else
                        zugzone_du[pos2] += 1000;
                    if (kingzone[pos2] == -farbvorzeichen) {
                        double angriffs_multiplikator = 1.0;
                        if (farbvorzeichen == _eigene_farbe) {
                            if (farbvorzeichen == 1) {

                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_w += KSafety * K_Angriff_Springer * angriffs_multiplikator;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_w += 1;
                            } else {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_s += KSafety * K_Angriff_Springer * angriffs_multiplikator;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_s += 1;
                            };
                            C_flag = 1;
                        } else {
                            if (farbvorzeichen == 1) {

                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_w += KSafety * K_Angriff_Springer * angriffs_multiplikator * 0.5;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_w += 1;
                            } else {
                                if ((feld[pos2+10*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+10*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                        feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 2.0;
                                } else if ((feld[pos2+11*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+11*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else if ((feld[pos2+9*farbvorzeichen] == W_K * -farbvorzeichen || feld[pos2+9*farbvorzeichen] == W_Kr * -farbvorzeichen) &&
                                           feld[pos2] != W_Bx * -farbvorzeichen && feld[pos2] != W_B * -farbvorzeichen) {
                                    angriffs_multiplikator = 1.8;
                                } else
                                    angriffs_multiplikator = 1.0;
                                Angreifer_Wert_s += KSafety * K_Angriff_Springer * angriffs_multiplikator * 0.5;
                                if (C_flag == 0)
                                    Anzahl_Angreifer_s += 1;
                            };
                            C_flag = 1;
                        }
                    }



                    //     if (kingzone[i] == 1) Attack_Pferd += KSafety;
                    if (zielfeld != LEER)  {
                        int zielfigur = abs(zielfeld);



                        /*   if (abs(zielfeld) == W_K)
                            break;*/

                        if (farbvorzeichen != _eigene_farbe)  {
                            if (zielfeld / _eigene_farbe > 0) { // Gegner greift meine Figur an
                                if (schlagzone_gegner[pos2] != 1)
                                    schlagzone_gegner[pos2] = 1;
                                else
                                    Attack_Pferd += KooIch;
                                //      Attack_Pferd += kingzone_ich[pos2]/* * Koenigsangriff_Er*/;
                                Attack_Pferd += (abs(zielfeld) * materialwert[abs(zielfeld)] - 40) * AttackIch;
                            } else {
                                Attack_Pferd += DefIch1;

                                if (abs(zielfeld) < 6)
                                    Attack_Pferd -= DefIch2;
                            } // Gegner deckt seine Figuren 1
                        } else  {
                            if (zielfeld / _eigene_farbe < 0) {                      // Ich greife Gegner an
                                if (schlagzone_ich[pos2] != 1)
                                    schlagzone_ich[pos2] = 1;
                                else
                                    Attack_Pferd += KooEr;

                                if (zielfigur == W_K || zielfigur == W_Kr)
                                    break;
                                //      Attack_Pferd += kingzone_gegner[pos2] * Koenigsangriff_Ich;
                                Attack_Pferd += (abs(zielfeld) * materialwert[abs(zielfeld)]) / AttackEr;
                            } else  {
                                Attack_Pferd += DefEr1;

                                if (abs(zielfeld) < 6)
                                    Attack_Pferd -= DefEr2;
                            }
                        } // Ich decke meine Figuren
                        break;
                    }

                    /*    if (farbvorzeichen != _eigene_farbe)  {
                        //        Attack_Pferd += kingzone_ich[pos2]/* * Koenigsangriff_Er*/;

                    //  }
                    //  else  {
                    //   Attack_Pferd += kingzone_gegner[pos2] * Koenigsangriff_Ich;
                    //   }//*/
                }
            }
            if (((feld[i+31*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+11*farbvorzeichen]==LEER)&&(feld[i+21*farbvorzeichen]==LEER)||(feld[i+29*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+19*farbvorzeichen]==LEER)&&(feld[i+29*farbvorzeichen]==LEER)||(feld[i+21*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+21*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+11*farbvorzeichen]==LEER))||((feld[i+19*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+19*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+9*farbvorzeichen]==LEER)))
                FS_Pferd -= Figurensicherheit;

            Attack_Pferd *= farbvorzeichen;
            FS_Pferd *= farbvorzeichen;

            n += AttSpr * Attack_Pferd + FS_Pferd;
        }

        if (((figur == W_B) || (figur == W_Bx))) {
            int Attack_Bauer = 0;

            /*   if (farbvorzeichen == 1)
                        {if (OpenLines_weiss[i%10-2] == 1 && OpenLines_weiss[i%10] == 1) Attack_Bauer -= 200;}
                else {if (OpenLines_schwarz[i%10-2] == 1 && OpenLines_schwarz[i%10] == 1) Attack_Bauer -= 200;}*/
            // if (feld[i+1] == feld[i]) n += 200;
            for (int richtung = 0; richtung <= bewegung[13][0]; richtung++)  {
                for (int weite = 0; weite <= bewegung[13][1]; weite++)  {
                    pos2 = i + farbvorzeichen * bewegung[13][2 + richtung] * (weite + 1);
                    int zielfeld = feld[pos2];
                    if (zielfeld == RAND) // Aus!
                        break;

                    if (farbvorzeichen == _eigene_farbe)
                        zugzone_ich[pos2] += 10000;
                    else
                        zugzone_du[pos2] += 10000;
                    if (zielfeld != LEER)  {
                        int zielfigur = abs(zielfeld);



                        /*   if (abs(zielfeld) == W_K)
                                break;*/
                        if (farbvorzeichen != _eigene_farbe)  {
                            if (zielfeld / _eigene_farbe > 0) { // Gegner greift meine Figur
                                // an
                                if (schlagzone_gegner[pos2] != 1)
                                    schlagzone_gegner[pos2] = 1;
                                else
                                    Attack_Bauer += KooIch;
                                //         Attack_Bauer += kingzone_ich[pos2]/* * Koenigsangriff_Er*/;
                                Attack_Bauer += (abs(zielfeld) * materialwert[abs(zielfeld)] - 40) * AttackIch;
                            } else {
                                Attack_Bauer += DefIch1 / 2;
                                //         if (abs(zielfeld) < 6) Attack_Bauer -= DefIch2;
                            }                                   // Gegner deckt seine Figuren
                            //    1
                        } else  {
                            if (zielfeld / _eigene_farbe < 0) { // Ich greife Gegner an
                                if (schlagzone_ich[pos2] != 1)
                                    schlagzone_ich[pos2] = 1;
                                else
                                    Attack_Bauer += KooEr;

                                if (zielfigur == W_K || zielfigur == W_Kr)
                                    break;
                                //        Attack_Bauer += kingzone_gegner[pos2] * Koenigsangriff_Ich;
                                Attack_Bauer += (abs(zielfeld) * materialwert[abs(zielfeld)]) / AttackEr;
                            } else {
                                Attack_Bauer += DefEr1 / 2;
                                //         if (abs(zielfeld) < 6) Attack_Bauer -= DefIch2;
                            }
                        } // Ich decke meine Figuren
                        break;
                    }

                }
            }
            Attack_Bauer *= farbvorzeichen;

            n += AttBau * Attack_Bauer;
        }

        if (((figur == W_K) || (figur == W_Kr))) {
            int Attack_Koenig = 0;
            int FS_Koenig = 0;
            //  int KSafety = 0;

            for (int richtung = 0; richtung <= bewegung[figur][0]; richtung++)  {
                for (int weite = 0; weite <= bewegung[figur][1]; weite++)  {
                    pos2 = i + farbvorzeichen * bewegung[figur][2 + richtung] * (weite + 1);
                    int zielfeld = feld[pos2];

                    if (zielfeld == RAND) // Aus!
                        break;
                    if (farbvorzeichen == _eigene_farbe)
                        zugzone_ich[pos2] += 1;
                    else
                        zugzone_du[pos2] += 1;

                    if (zielfeld != LEER)  {
                        int zielfigur = abs(zielfeld);

                        if (farbvorzeichen != _eigene_farbe)  {

                            if (zielfeld / _eigene_farbe > 0) { // Gegner greift meine Figur an
                                if (schlagzone_gegner[pos2] != 1)
                                    schlagzone_gegner[pos2] = 1;
                                else
                                    Attack_Koenig += KooIch;
                                //    Attack_Koenig += kingzone_ich[pos2]/* * Koenigsangriff_Er*/;
                                Attack_Koenig += (abs(zielfeld) * materialwert[abs(zielfeld)] - 40) * AttackIch;
                            } else {
                                Attack_Koenig += DefIch1;//if (abs(zielfeld)<6) Attack_Koenig -= DefIch2;
                            }
                            // Gegner deckt seine Figuren    1
                        } else  {
                            if (zielfeld / _eigene_farbe < 0) { // Ich greife Gegner an
                                if (schlagzone_ich[pos2] != 1)
                                    schlagzone_ich[pos2] = 1;
                                else
                                    Attack_Koenig += KooEr;

                                if (zielfigur == W_K)
                                    break;
                                //      Attack_Koenig += kingzone_gegner[pos2] * Koenigsangriff_Ich;
                                Attack_Koenig += (abs(zielfeld) * materialwert[abs(zielfeld)]) / AttackEr;
                            } else  {
                                Attack_Koenig += DefEr1;//if (abs(zielfeld)<6) Attack_Koenig -= DefEr2;
                            }

                        } // Ich decke meine Figuren
                        break;
                    }
                }
            }
            if (((feld[i+31*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+11*farbvorzeichen]==LEER)&&(feld[i+21*farbvorzeichen]==LEER)||(feld[i+29*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+19*farbvorzeichen]==LEER)&&(feld[i+29*farbvorzeichen]==LEER)||(feld[i+21*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+21*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+11*farbvorzeichen]==LEER))||((feld[i+19*farbvorzeichen] == -W_B*farbvorzeichen || feld[i+19*farbvorzeichen] == -W_Bx*farbvorzeichen)&&(feld[i+9*farbvorzeichen]==LEER)))
                FS_Koenig -= Figurensicherheit;
            Attack_Koenig *= farbvorzeichen;
            FS_Koenig *= farbvorzeichen;

            n += AttKoe * Attack_Koenig + FS_Koenig/* + KSafety*/; // + KSafety;
        }
    }

    static const double koenigssicherheits_faktor[9] = {
        0.0,        // 0 Angreifer (Sonderfall: wir setzen die Wirkung auf 0)
        0.0,        // 1 Angreifer
        0.50,       // 2 Angreifer
        0.75,       // 3 Angreifer
        0.875,      // 4 Angreifer
        0.9375,     // 5 Angreifer
        0.96875,    // 6 Angreifer
        0.984375,   // 7 Angreifer
        0.99        // 8 oder mehr Angreifer
    };


    // Anzahl Angreifer in sinnvollen Bereich [0..8] einklemmen
    int anzahl_w = Anzahl_Angreifer_w;
    if (anzahl_w < 0)
        anzahl_w = 0;
    if (anzahl_w > 8)
        anzahl_w = 8;

    int anzahl_s = Anzahl_Angreifer_s;
    if (anzahl_s < 0)
        anzahl_s = 0;
    if (anzahl_s > 8)
        anzahl_s = 8;


    double faktor_w = koenigssicherheits_faktor[anzahl_w];
    double faktor_s = koenigssicherheits_faktor[anzahl_s];

    K_Safety_Wert = Angreifer_Wert_w * faktor_w
                    - Angreifer_Wert_s * faktor_s;

    n += K_Safety_Wert;

    int Koordination_ich = 0;
    int Koordination_du = 0;
    int Kontrolle_ich = 0;
    int Kontrolle_du = 0;

    for (int i=21; i < 99; i++) {

        if (zugzone_ich[i] > zugzone_du[i] && (i-60)/_eigene_farbe > 0) {
            Kontrolle_ich += 1;
            if (feld[i] != 0 && feld[i]/abs(feld[i]) != _eigene_farbe)
                Kontrolle_ich += 1;
            if ((zugzone_ich[i+1] > zugzone_du[i+1]) || (zugzone_ich[i-1] > zugzone_du[i-1] ) || (zugzone_ich[i+10] > zugzone_du[i+10] ) || (zugzone_ich[i-10] > zugzone_du[i-10] ))
                Koordination_ich += 1;
            if ((zugzone_ich[i+1] == zugzone_du[i+1]) || (zugzone_ich[i-1] == zugzone_du[i-1] ) || (zugzone_ich[i+10] == zugzone_du[i+10] ) || (zugzone_ich[i-10] == zugzone_du[i-10] ))
                Koordination_ich += 0.5;
        }
        if (zugzone_du[i] > zugzone_ich[i] && (i-60)/_eigene_farbe < 0) {
            Kontrolle_du += 1;
            if (feld[i] != 0 && feld[i]/abs(feld[i]) == _eigene_farbe)
                Kontrolle_du += 1;
            if ((zugzone_du[i+1] > zugzone_ich[i+1]) || (zugzone_du[i-1] > zugzone_ich[i-1] ) || (zugzone_du[i+10] >  zugzone_ich[i+10] ) || (zugzone_du[i-10] > zugzone_ich[i-10] ))
                Koordination_du += 1;
            if ((zugzone_du[i+1] == zugzone_ich[i+1]) || (zugzone_du[i-1] == zugzone_ich[i-1] ) || (zugzone_du[i+10] == zugzone_ich[i+10] ) || (zugzone_du[i-10] == zugzone_ich[i-10] ))
                Koordination_du += 0.5;
        }

    }

    n += (Koordination_ich - Koordination_du) * _eigene_farbe * 58 + (Kontrolle_ich - Kontrolle_du) * _eigene_farbe * Kontrolle;
    return n;
}

int sort(denkpaar _zugstapel[200], int _n, int _stufe, int _i) {
    // --- Phase 1: Vorbereitung der 'order'-Werte (nur einmal ganz am Anfang) ---
    if (_i == 0) {
        for (int k = 0; k < _n; k++) {
            denkpaar& zug = _zugstapel[k];

            // Priorität 1: Gibt es in der Transposition Table schon einen best move?
             if (ttMoveId[_stufe] != 0 && zug.z.id == ttMoveId[_stufe]) {
                zug.order = 4000000;
                continue; // Nächster Zug
            }

            // Priorität 2: Ist es der PV-Zug? (Immer ganz oben)
            if (zug.z.id != 0 && zug.z.id == best_one[_stufe].z.id) {
                zug.order = 3000000;
                continue;
            }

            // Priorität 3: Ist es ein Schlagzug?
            if (zug.kill) {
                // MVV/LVA ist der Basiswert. Wir packen ihn in einen hohen "Bucket".
                // Der Wert 2.000.000 sorgt dafür, dass er über allen ruhigen Zügen steht.
                zug.order = 2000000 + zug.order;
                continue;
            }

            // Priorität 4: Ist es ein Killer-Zug?
            if (zug.z.id != 0 && zug.z.id == killerMoves[_stufe][0].z.id) {
                zug.order = 1000002; // Killer 1 ist besser als Killer 2
                continue;
            }
            if (zug.z.id != 0 && zug.z.id == killerMoves[_stufe][1].z.id) {
                zug.order = 1000001; // Killer 2
                continue;
            }

            // Priorität 5: Alle anderen ruhigen Züge (werden nach History sortiert)
            // Wir nehmen den vollen History-Wert.
            zug.order = historyMoves[zug.z.pos.pos1][zug.z.pos.pos2];
        }
    }

    // --- Phase 2: Eigentliche iterative Sortierung ---
    int best_index = _i;

    // Durchlaufe alle Züge ab Position _i.
    for (int j = _i; j < _n; ++j) {
        // Wenn der aktuelle Zug (nach Aktualisierung seines 'order'-Werts)
        // besser ist als der bisher beste gefundene Zug, merke dir seinen Index.
        if (_zugstapel[j].order > _zugstapel[best_index].order) {
            best_index = j;
        }
    }

    // Nachdem alle Züge geprüft wurden, tausche den besten gefundenen Zug
    // (an _zugstapel[best_index]) an die Position _zugstapel[_i],
    // falls er nicht schon dort ist.
    if (best_index != _i) {
        std::swap(_zugstapel[_i], _zugstapel[best_index]);
    }

    return 0;
}
