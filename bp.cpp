// --- Globale Variablen für die Suche ---
feldtyp * testbrett[ende+1];
Spielfeld * testspiel[ende+1];
howitends __end = nothing;

struct ZugPosition {
    int pos1;
    int pos2;
};

ZugPosition letzter_zug_weiss = {0, 0};
ZugPosition letzter_zug_schwarz = {0, 0};

ZugPosition letzter_zug_weiss_prev = {0,0};
ZugPosition letzter_zug_schwarz_prev = {0,0};

int bp (Spielfeld & spiel, int farbe, int alpha, double beta, int stufe, int _stopp, int NullFlag) { // Bewertung, Planung


    if (NullFlag == 3) {
        spiel.Farbe = -farbe;
    } else
        spiel.Farbe = farbe;//*/

    double wertung = 0;
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = true;
    }

    spiel.makeZugstapel();

    spiel.find_kings();
    int king = (farbe > 0) ? spiel.wking : spiel.bking;
    const bool inCheckNow = spiel.test_drohung(Feld[spiel.getStufe()], farbe, king);
    const int ext = (inCheckNow && (_stopp < ende - 1)) ? 1 : 0;    // Check Extension

    int n = spiel.n;  // Anzahl der Zuege
    int nn = 0;       // Anzahl der vom Schach her machbaren Zuege


// ==========================================================
// =========== START DER ZUG-SCHLEIFE (for-loop) ============
// ==========================================================
    for (int i=0; i < n; i++) {

        // LATE MOVE PRUNING
        if (!inCheckNow && stufe > 3 && i > 20 && !zugstapel[spiel.getStufe()][i].kill) {
            if ((_stopp - stufe) < 2) { // In geringer Tiefe
                continue; // Überspringe späte, ruhige Züge
            }
        }

        // --- Zug ausführen und auf Legalität prüfen ---
        sort(zugstapel[spiel.getStufe()], spiel.n, stufe, i);
        testspiel[stufe]->copy(spiel);
        testspiel[stufe]->zug(zugstapel[spiel.getStufe()][i]);

        __end = testspiel[stufe]->last_moves();
        if (__end != nothing) {
            int terminal_wertung; // Eine separate Variable, um nicht mit der normalen 'wertung' zu kollidieren

            if (__end == schachmatt) {
                terminal_wertung = MAX_WERT - stufe;   // Wir haben gewonnen!
            } else if (__end == matt) {
                terminal_wertung = -(MAX_WERT - stufe); // Wir haben verloren!
            } else if (__end == patt || __end == remis) {
                terminal_wertung = 0; // Remis
            } else {
                continue; // Das ist ein wirklich illegaler Zug, überspringen.
            }

            // Integriere das Ergebnis in die Alpha-Beta-Suche
            zugstapel[spiel.getStufe()][i].bewertung = terminal_wertung;
            if (terminal_wertung > alpha) {
                bester_zug[stufe] = zugstapel[spiel.getStufe()][i];
                best_one[stufe]   = zugstapel[spiel.getStufe()][i];

                if (terminal_wertung >= beta) {
                    spiel.nn = nn; // Wichtig: nn vor dem Return aktualisieren
                    return beta;   // Beta-Cutoff
                }
                alpha = terminal_wertung;
            }
            continue; // Nach der Bewertung des Endzustands mit dem nächsten Zug in der Schleife fortfahren.
        }

        nn += 1; // Nur wenn die Partie NICHT zu Ende ist, zählen wir den legalen Zug


        aktueller_zug[stufe] = zugstapel[spiel.getStufe()][i];


        if (((stufe +1>= _stopp + ext)||(stufe+1 >= ende))) {
            // Stellungsbewertung im Blatt (gewünschte Tiefe erreicht)
            wertung = rand() % 3 - 1;
            //wertung = 0;

            wertung += (double) 1.55 * material   (Feld[testspiel[stufe]->getStufe()], farbe); //8.75-9		90
            if (alpha < wertung*farbe + 180) {
                wertung += (double) 1.55 *  entwicklung(Feld[testspiel[stufe]->getStufe()], farbe);		//0.375-0.4		-->160		1.6
                wertung += (double) 0.09 *  zuganzahl  (Feld[testspiel[stufe]->getStufe()], farbe); //0,8;0.076

            }


            if ((wertung*farbe > alpha-50 && wertung*farbe < beta + 500) && aktueller_zug[stufe].kill && stufe < _stopp + 2 && (stufe + 1 < ende)) {

                wertung = - bp(*testspiel[stufe], farbe*-1, -beta, -alpha, stufe + 1, _stopp, 1);

            } else {
                wertung = wertung * (farbe);
                if (farbe == 1 ) {
                    wertung -= 10;
                }
            }
        }



        else {

            if (!inCheckNow && (NullFlag==1) && (_stopp-stufe)>2) {

                int wertungn = 0;

                wertungn = - bp(*testspiel[stufe], farbe, -beta, -beta+1, stufe + 1, _stopp-2, 3);


                if (wertungn >= beta && std::abs(beta)!= MAX_WERT ) {
                    return beta;
                }

            }


            if (NullFlag==1) {
                if ((_stopp-stufe)>2) {
                    if (!inCheckNow && i > 4 && !aktueller_zug[stufe].kill) {

                        wertung = - bp(*testspiel[stufe], farbe*-1, -alpha-1, -alpha, stufe + 1, _stopp-2, 4);

                    } else
                        wertung = alpha + 1;


                    if(wertung > alpha) {

                        wertung = - bp(*testspiel[stufe], -farbe, -beta, -alpha, stufe + 1, _stopp + ext, 4);
                    }
                } else {
                    if (!inCheckNow && i > 4 && (_stopp-stufe > 2) && !aktueller_zug[stufe].kill) {

                        wertung = - bp(*testspiel[stufe], farbe*-1, -alpha-1, -alpha, stufe + 1, _stopp-2, 1);

                    } else
                        wertung = alpha + 1;


                    if(wertung > alpha) {
                        wertung = - bp(*testspiel[stufe], farbe*-1, -beta, -alpha, stufe + 1, _stopp + ext, 1);
                    }
                }
            } else  {
                if (!inCheckNow && i > 4 && (_stopp-stufe > 2) && !aktueller_zug[stufe].kill) {

                    wertung = - bp(*testspiel[stufe], farbe*-1, -alpha-1, -alpha, stufe + 1, _stopp-2, 2);

                } else
                    wertung = alpha + 1;


                if(wertung > alpha) {
                    wertung = - bp(*testspiel[stufe], farbe*-1, -beta, -alpha, stufe + 1, _stopp + ext, 2);


                    zugstapel[spiel.getStufe()][i].bewertung = wertung;
                } //}
            }
        }

        // ===== HIN-UND-HER-ERKENNUNG (nur auf Root-Ebene) =====
        if (stufe == 0) {
            bool quiet_move = !zugstapel[spiel.getStufe()][i].kill;

            if (quiet_move) { // Nur bei ruhigen Zügen
                ZugPosition& lastSelf = (farbe == 1) ? letzter_zug_weiss : letzter_zug_schwarz;

                // Gegner: letzte zwei Züge (C->D und davor D->C?)
                ZugPosition& oppLast  = (farbe == 1) ? letzter_zug_schwarz : letzter_zug_weiss;
                ZugPosition& oppPrev  = (farbe == 1) ? letzter_zug_schwarz_prev : letzter_zug_weiss_prev;

                // Mein Kandidat ist direkte Rücknahme? (B->A)
                bool selfBack =
                    (lastSelf.pos1 != 0) &&
                    (aktueller_zug[0].z.pos.pos1 == lastSelf.pos2) &&
                    (aktueller_zug[0].z.pos.pos2 == lastSelf.pos1);

                // Gegner zeigt bereits Ping-Pong? (… C->D und davor D->C)
                bool oppPingPong =
                    (oppLast.pos1 != 0) && (oppPrev.pos1 != 0) &&
                    (oppLast.pos1 == oppPrev.pos2) &&
                    (oppLast.pos2 == oppPrev.pos1);

                // Nur wenn beides gilt, blocken/abwerten:
                if (selfBack && oppPingPong) {
                    wertung = 0;
                }
            }
        }

        /*  if (testspiel[stufe]->spezial == SCHACH) {
              testspiel[stufe]->spezial = NICHTS;
              continue;
          }*/

        zugstapel[spiel.getStufe()][i].bewertung = wertung;


        if (stufe==0 && _stopp==stopp) {
            bewertet++;;
            cout << ""
                 << grundfeld_bezeichnungen[aktueller_zug[0].z.pos.pos1]
                 << " => "	<< grundfeld_bezeichnungen[aktueller_zug[0].z.pos.pos2]
                 << " Bewertung: "
                 << setw(5) << wertung
                 <<", Zug-ID "
                 << setw(6) << aktueller_zug[0].z.id << "\n";
            cout.flush();
        }
        //else { if (stufe == 0) cout << "*" << flush;}

        if (wertung > alpha) {

            bester_zug[stufe] = zugstapel[spiel.getStufe()][i];
            best_one[stufe] = zugstapel[spiel.getStufe()][i]; //Aktueller PV-Zug f�r jede Stufe inkl. Zuginformationen
            //  best_one[stufe].bewertung *= 0.5; //ACHTUNG 5

            if (wertung >= beta) {

                if(!aktueller_zug[stufe].kill) {
                    int from = aktueller_zug[stufe].z.pos.pos1;
                    int to = aktueller_zug[stufe].z.pos.pos2;
                    if ((unsigned)from < 120u && (unsigned)to < 120u) {
                        historyMoves[from][to] += (_stopp - stufe) * (_stopp - stufe);
                    }
                    // KILLER MOVES
                    // Konkrete Züge, die auf einer konkreten Stufe für Cutoffs gesorgt haben, merken wir uns für die Sortierung
                    if (zugstapel[spiel.getStufe()][i].z.id != killerMoves[stufe][0].z.id) {
                        killerMoves[stufe][1] = killerMoves[stufe][0];
                        killerMoves[stufe][0] = zugstapel[spiel.getStufe()][i];
                    }
                }


                spiel.nn = nn;

                return beta;
            }

            alpha = wertung;


        }
    }

// ===== PATT/MATT-ERKENNUNG  =====
    if (nn == 0) {
        // Prüfe DIREKT ob im Schach:

        spiel.find_kings();
        int king = (farbe > 0) ? spiel.wking : spiel.bking;

        // KRITISCH: Welche Semantik hat test_drohung?
        // Test beide Varianten:
        bool check1 = spiel.test_drohung(Feld[spiel.getStufe()], farbe, king);

        return check1 ? -(MAX_WERT - stufe) : 0;  // Variante 1
    }

    spiel.nn = nn;

    return alpha;

}
