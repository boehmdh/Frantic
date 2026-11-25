#include "MainGamePanel.h"
#include "../uiElements/ImagePanel.h"
#include "../GameController.h"


MainGamePanel::MainGamePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition),
                                                 eventBannerTimer(this)
{

	outerLayout = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(outerLayout);
    this->SetMinSize(panelSize);
    laF.SetPixelSize(la); // Large font 32 pixel high
}

void MainGamePanel::buildGameState(game_state* gameState, player* me) {
    currentGameState = gameState;
    currentPlayer = me;
	//stop any running banner
    eventBannerTimer.Stop();
    eventBanner = nullptr;
    eventBannerBlinkCount = 0;
    // remove any existing UI
    this->DestroyChildren();
    this->outerLayout = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(outerLayout);
    outerLayout->AddSpacer(310);

    std::vector<player*> players = gameState->get_players();
    int numberOfPlayers = players.size();

    // find our own player object in the list of players
    int myPosition = -1;
    std::vector<player*>::iterator it = std::find_if(players.begin(), players.end(), [me](const player* x) {
        return x->get_id() == me->get_id();
    });
    if (it < players.end()) {
        me = *it;
        myPosition = it - players.begin();
    } else {
        GameController::showError("Game state error", "Could not find this player among players of server game.");
        return;
    }

    double anglePerPlayer = MainGamePanel::twoPi / (double) numberOfPlayers;

    // show all other players
    for(int i = 1; i < numberOfPlayers; i++) {

        // get player at i-th position after myself
        player* otherPlayer = players.at((myPosition + i) % numberOfPlayers);
        double playerAngle = (double) i * anglePerPlayer;
        this->buildOtherPlayerHand(gameState, otherPlayer, playerAngle);
    }

    // show all 3 card piles at the center
    this->buildCardPiles(gameState, me);

    // show our own player
    this->buildThisPlayer(gameState, me);

    // update layout
    this->Layout();
}

void MainGamePanel::buildOtherPlayerHand(game_state* gameState, player* otherPlayer, double playerAngle) {
    // get this player's position on that ellipse
    wxPoint handPosition = MainGamePanel::tableCenter;
    wxPoint labelPosition = MainGamePanel::tableCenter - wxSize(130, 0); // hor. offset for centering
    handPosition += this->getPointOnEllipse(oPlHaDist * 1.55, oPlHaDist, playerAngle);

    // the labels are shifted in order to be displayed above the player's hands and to the right of the opposite one
    double labelAngle = ((playerAngle < 3.1 ? 1 : -1) * 0.04 + cos(playerAngle/2) * 0.05) * twoPi + playerAngle;
    labelPosition += this->getPointOnEllipse(oPlLaDist * 1.18, oPlLaDist, labelAngle);

    // add image of player's hand
    int numberOfCards = otherPlayer->get_nof_cards();
    if(numberOfCards > 0) {
        // get new bounds of image, as they increase when image is rotated
        wxSize boundsOfRotatedHand = this->getBoundsOfRotatedSquare(MainGamePanel::otherPlayerHandSize, playerAngle);
        handPosition -= boundsOfRotatedHand / 2;

        std::string handImage = "assets/frantic_hand_" + std::to_string(numberOfCards) + ".png";
        if(numberOfCards > 10) {
            handImage = "assets/frantic_hand_10.png";
        }
        wxSize nonRotatedSize = wxSize((int) MainGamePanel::otherPlayerHandSize, (int) MainGamePanel::otherPlayerHandSize);
        new ImagePanel(this, handImage, wxBITMAP_TYPE_ANY, handPosition, nonRotatedSize, playerAngle);
    } else if(numberOfCards == 0) {
        wxSize nonRotatedSize = wxSize((int) MainGamePanel::otherPlayerHandSize, (int) MainGamePanel::otherPlayerHandSize);
        handPosition -= nonRotatedSize / 2;
        new ImagePanel(this, "assets/cat.png", wxBITMAP_TYPE_ANY, handPosition, nonRotatedSize);
    }

    //Player label
    std::string label = otherPlayer->get_player_name();
    std::string otherPoints = "waiting...";
    if (gameState->is_started()) {
        otherPoints = std::to_string(otherPlayer->get_score()) + " points";
    }
    label.append("\n" + otherPoints);
    wxStaticText* oPlLabel = new wxStaticText(this, -1, label, labelPosition, labelTWin, wxALIGN_CENTRE);
    oPlLabel->SetFont(otherPlayer == gameState->get_current_player() ? laF.Bold() : laF);

}


static std::string getCardImagePath(const card* c) {
    uint value = c->get_value();
    Color color = c->get_color();
    int colorIndex = static_cast<uint>(color);

    if (value == 0 && c->get_action() != CardActionType::NONE) {
        std::string actionStr = action_to_string(c->get_action());

        //COLOR_action.png
        return "assets/special_" + std::to_string(colorIndex) + "_" + actionStr + ".png";
    }

    // normale Nummernkarten
    return "assets/frantic_card_" +
           std::to_string(colorIndex) + "_" +
           std::to_string(value) + ".png";
}

/**
 * @brief Displays the card piles in the main game panel.
 *
 * This function constructs the visual layout for the card piles including:
 * - Discard pile: shows the top card of the discard pile.
 * - Draw pile: shows the back of the draw pile. If it is the current player's turn
 *   and they haven’t drawn yet, the draw pile can be clicked to draw a card.
 * - Event pile: shows the top card of the event discard pile. If a new event card appears, a message box
 *   is shown describing the event.
 * - Event discard pile.
 *
 * If the game has not started yet, a single card back is shown in the center.
 *
 * @param gameState Pointer to the current game state object, used to get information
 *                  about piles and the current player.
 * @param me Pointer to the current player object.
 */
void MainGamePanel::buildCardPiles(game_state* gameState, player *me) {
    wxBoxSizer* cardPileLayout = new wxBoxSizer(wxHORIZONTAL);
    outerLayout->Add(cardPileLayout, 1, wxALIGN_CENTRE);

    if(gameState->is_started()) {
        // Show discard pile
		const card* topCard = gameState->get_discard_pile()->get_top_card();
		if (topCard != nullptr) {
				    std::string cardImage = getCardImagePath(topCard);
					ImagePanel* discardPile = new ImagePanel(this,cardImage,wxBITMAP_TYPE_ANY, wxDefaultPosition, cardSize);
                    cardPileLayout->Add(discardPile, 0, wxALIGN_CENTRE);
					discardPile->SetToolTip("Discard pile");
		            cardPileLayout->AddSpacer(30);
			}


        // Show draw pile
        ImagePanel* drawPile = new ImagePanel(this, "assets/frantic_back.png", wxBITMAP_TYPE_ANY, wxDefaultPosition, cardSize);
        cardPileLayout->Add(drawPile,1, wxALIGN_CENTRE, 0);
        if(gameState->get_current_player() == me &&!me->has_drawn()) {
            drawPile->SetToolTip("Draw card");
            drawPile->SetCursor(wxCursor(wxCURSOR_HAND));
            drawPile->Bind(wxEVT_LEFT_UP, [](wxMouseEvent& event) {
                GameController::drawCard();
            });
        } else {
            drawPile->SetToolTip("Draw pile");
        }

        // Show event pile
        const event_card* topEventCard = gameState->get_event_discard_pile()->get_top_card();
        // default image until the first event card is uncovered
        std::string cardImage = "assets/frantic_back_event.png";
        if (topEventCard != nullptr) {
            Event_card_collection eventName = topEventCard->get_event_name();
            cardImage =
                  "assets/" + getEventCardString(eventName) + ".png";

            static std::string lastShownEventId;
            std::string currentId = topEventCard->get_id();

            if (currentId != lastShownEventId) {
                lastShownEventId = currentId;

                wxString msg;
                switch (eventName) {
                    case Event_card_collection::EARTHQUAKE:
                        msg = "Earthquake!\nAll players pass their hands to the right.";
                        break;
                    case Event_card_collection::CAPITALISM:
                        msg = "Capitalism!\nEvery player draws as many cards as they already had.";
                        break;
                    case Event_card_collection::DOOMSDAY:
                        msg = "Doomsday!\nThe round ends immediately. Everyone gets 50 points.";
                        break;
                    case Event_card_collection::ROBIN_HOOD:
                        msg = "Robin Hood!\nThe player with most cards and the player with the fewest cards swap hands.";
                        break;
                    case Event_card_collection::COMMUNISM:
                        msg = "Communism!\nDraw cards from the draw pile until you all have the same number of cards as the person with the most cards in their hand.";
                        break;
                    case Event_card_collection::EXPANSION:
                        msg = "Expansion!\nIn order of play: first draws one card, second draws two, third draws three, and so on.";
                        break;
                    case Event_card_collection::THIRD_TIME_LUCKY:
                        msg = "Third Time Lucky!\nDraw three cards from the draw pile.";
                        break;
                    case Event_card_collection::MEXICAN_STANDOFF:
                        msg = "Mexican Standoff!\nDiscard your entire hand of cards and draw three new cards from the pile in the order of play.";
                        break;
                    case Event_card_collection::MATING_SEASON:
                        msg = "Mating Season!\nDiscard all number cards with a numerical value that you have at least twice in your hand. This also applies to black number cards.";
                        break;
                    case Event_card_collection::FRIDAY_THE_13TH:
                        msg = "Friday the 13th!\nIt’s Friday, the Thirteenth. A hook-handed murderer is among us! But just in the movies, it’s a totally boring, normal Friday, nothing weird happens. The game continues without further ado.";
                        break;
                    case Event_card_collection::TIME_BOMB:
                        msg = "Time Bomb!\nEveryone only has three turns left, as the round ends then at the very latest. If someone ends the round before that, ten points are subtracted from their total score and the rest add ten points to the score for their hands of cards. If no one manages to end the round within three turns, all players’ points are doubled.";
                        break;
                    case Event_card_collection::FINISH_LINE:
                        msg = "Finish Line!\nThe round ends immediately. Add together the points on your hand of ca rds.";
                        break;
                    default:
                        break;
                }

                if (!msg.IsEmpty()) {
                    wxMessageBox(msg, "Event card", wxOK | wxICON_INFORMATION, this);
                }
            }
        }
        ImagePanel* eventPile = new ImagePanel(this, cardImage, wxBITMAP_TYPE_ANY, wxDefaultPosition, cardSize);
		eventPile->SetToolTip("Event pile");
        cardPileLayout->AddSpacer(50);
        cardPileLayout->Add(eventPile,1, wxALIGN_CENTRE);

    } else {
        // if the game did not start yet, show a back side of a card in the center (only for the mood)
        ImagePanel* cardPile = new ImagePanel(this, "assets/frantic_back.png", wxBITMAP_TYPE_ANY, wxDefaultPosition, MainGamePanel::cardSize);
        outerLayout->Add(cardPile, 0, wxALIGN_CENTRE | wxALIGN_TOP, 20);
    }

}

void MainGamePanel::buildThisPlayer(game_state* gameState, player* me) {
    //Setup two nested box sizers, in order to align our player's UI to the bottom center
    //wxBoxSizer* outerLayout = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* innerLayout = new wxBoxSizer(wxVERTICAL);
    outerLayout->Add(innerLayout, 0, wxBOTTOM | wxCENTRE);
    innerLayout->AddSpacer(40);


    // Show the label with our player name
    wxStaticText* pN = new wxStaticText(this, -1, me->get_player_name(),wxDefaultPosition, lTWin, wxALIGN_CENTRE);
    pN->SetFont(laF);
    innerLayout->Add(pN, 0, wxALIGN_CENTER | wxALL, 10);

    // if the game has not yet started we say so
    if(!gameState->is_started()) {

        wxStaticText* pP = new wxStaticText(this, -1, "waiting for game to start...",wxDefaultPosition, lTWin, wxALIGN_CENTRE);

        pP->SetFont(laF);
        innerLayout->Add(pP, 0, wxALIGN_CENTER | wxBOTTOM, 8);

        // show button that allows our player to start the game
        wxButton* startGameButton = new wxButton(this, wxID_ANY, "Start Game!", wxDefaultPosition, wxSize(160, 64));
        startGameButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
            GameController::startGame();
        });
        innerLayout->Add(startGameButton, 0, wxALIGN_CENTER | wxBOTTOM, 8);

    } else {
        std::string tIndT = "It's your turn!"; //turn indicator text initialization
        // show our player's points
        wxStaticText* pP = new wxStaticText(this, -1, std::to_string(me->get_score()) + " points" ,wxDefaultPosition, lTWin, wxALIGN_CENTRE);

        pP->SetFont(laF);
        innerLayout->Add(pP, 0, wxALIGN_CENTER | wxBOTTOM, 8);

        // if our player folded, we display that as status
        if (gameState->get_current_player() == me) {
            if (!me->has_drawn()) {
                wxStaticText *pStat = new wxStaticText(this, wxID_ANY, "Play a card or draw one",wxDefaultPosition, sTWin,wxALIGN_CENTER_HORIZONTAL);

                innerLayout->Add(pStat, 0, wxALIGN_CENTER | wxBOTTOM, 8);
                //we already draw one card and we need to fold
            } else {
                wxButton *foldButton = new wxButton(
                        this,
                        wxID_ANY,
                        "Fold",
                        wxDefaultPosition,
                        wxSize(80, 32)
                );
                foldButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
                    GameController::fold();
                });
                innerLayout->Add(foldButton, 0, wxALIGN_CENTER | wxBOTTOM, 8);
            }

        // if it's not our turn, display "waiting..."
        } else {
            wxStaticText *pStat = new wxStaticText(this, -1, "waiting...", wxDefaultPosition, sTWin, wxALIGN_CENTRE);
            innerLayout->Add(pStat, 0, wxALIGN_CENTER | wxBOTTOM, 8);
            tIndT = "It's " + gameState->get_current_player()->get_player_name() + "'s turn!";

        }

        // display our player's hand, if we have cards
        int numberOfCards = me->get_nof_cards();
        if (numberOfCards > 0) {

            // create horizontal layout for the individual hand cards of our player
            wxBoxSizer *handLayout = new wxBoxSizer(wxHORIZONTAL);
            innerLayout->Add(handLayout, 0, wxALIGN_CENTER);

            wxSize scaledCardSize = MainGamePanel::cardSize;

            // Adjust card size (if the number of cards does not fit on the screen)
            if (numberOfCards * (MainGamePanel::cardSize.GetWidth() + 8) >
                MainGamePanel::panelSize.GetWidth()) { // 8 -> 4 pixel padding on both sides
                int scaledCardWidth = (MainGamePanel::panelSize.GetWidth() / numberOfCards) - 8;
                double cardAspectRatio =
                        (double) MainGamePanel::cardSize.GetHeight() / (double) MainGamePanel::cardSize.GetWidth();
                int scaledCardHeight = (int) ((double) scaledCardWidth * cardAspectRatio);
                scaledCardSize = wxSize(scaledCardWidth, scaledCardHeight);
            }

            // Show all cards
						for (int i = 0; i < me->get_hand()->get_cards().size(); i++) {
								card* handCard = me->get_hand()->get_cards().at(i);
								std::string cardFile = getCardImagePath(handCard);

								ImagePanel* cardButton = new ImagePanel(
										this,
										cardFile,
										wxBITMAP_TYPE_ANY,
										wxDefaultPosition,
										scaledCardSize
								);

								if (gameState->get_current_player() == me ) {
										cardButton->SetToolTip("Play card");
										cardButton->SetCursor(wxCursor(wxCURSOR_HAND));
										cardButton->Bind(wxEVT_LEFT_UP, [handCard](wxMouseEvent& event) {
												GameController::playCard(handCard);
										});
								}
								int cardSpacing = scaledCardSize.GetWidth() / 50; // 1/5 of the card width
								handLayout->Add(cardButton, 0, wxLEFT | wxRIGHT, cardSpacing / 2);
						}
        }
        uint wish = gameState->get_wish();
        if (wish != 0) { //append active wish
            tIndT.append("      Active wish: ").append(wish > 10 ? getColorString(static_cast<Color>(wish)) : std::to_string(wish));
        }
        // Display turn indicator text if the game has started
        wxStaticText* tInd = new wxStaticText(this, -1, tIndT, wxDefaultPosition, lTWin, wxALIGN_CENTRE);
        tInd->SetFont(laF);
        outerLayout->AddSpacer(10);
        outerLayout->Add(tInd,0, wxALIGN_CENTER | wxBOTTOM, 5);
    }
}


/**
 * @brief Displays a dialog for the player to choose a color or number for a Rainbow card.
 *
 * If a Rainbow card is on top of the discard pile, this function asks the player
 * for a color or number for the wish. The choices for colors are
 * Red, Blue, Green, and Yellow. The number choices range from I to IX.
 *
 * @return uint The selected color (as Color enum) or number (1-9) chosen by the player.
 */
uint MainGamePanel::ShowWishColorMessage()
{
    wxArrayString choice;
    choice.Add("Red");
    choice.Add("Blue");
    choice.Add("Green");
    choice.Add("Yellow");
    choice.Add("I");
    choice.Add("II");
    choice.Add("III");
    choice.Add("IV");
    choice.Add("V");
    choice.Add("VI");
    choice.Add("VII");
    choice.Add("VIII");
    choice.Add("IX");

    wxSingleChoiceDialog dialog(
            this,
            "Choose a color for your wish card:",
            "Wish Color",
            choice
    );

    wxString selection;
    if (dialog.ShowModal() == wxID_OK) {
        selection = dialog.GetStringSelection();
    }

    uint wish = 0;
    if (selection == "Red") {wish = static_cast<uint>(Color::RED);}
    else if (selection == "Blue") {wish = static_cast<uint>(Color::BLUE);}
    else if (selection == "Green") {wish = static_cast<uint>(Color::GREEN);}
    else if (selection == "Yellow") {wish = static_cast<uint>(Color::YELLOW);}
    else if (selection == "I") {wish = 1;}
    else if (selection == "II") {wish = 2;}
    else if (selection == "III") {wish = 3;}
    else if (selection == "IV") {wish = 4;}
    else if (selection == "V") {wish = 5;}
    else if (selection == "VI") {wish = 6;}
    else if (selection == "VII") {wish = 7;}
    else if (selection == "VIII") {wish = 8;}
    else if (selection == "IX") {wish = 9;}

    return wish;
}


uint MainGamePanel::ShowFantasticFourPlayerMessage()
{
    wxArrayString names;
    std::vector<player*>& players = currentGameState->get_players();

    std::vector<int> indexMap;
		//std::string myId = currentPlayer->get_id();
    for (size_t i = 0; i < players.size(); ++i) {
        player* p = players[i];

        if (p->get_id() == currentPlayer->get_id())
            continue;

        names.Add(p->get_player_name());
        indexMap.push_back(i);
    }

    wxSingleChoiceDialog dlg(
        this,
        "Select a player who must draw four cards:",
        "Fantastic Four",
        names
    );

    if (dlg.ShowModal() != wxID_OK)
        return 0;

    int sel = dlg.GetSelection();
    return static_cast<uint>(indexMap[sel]);
}

uint MainGamePanel::ShowEqualityPlayerMessage()
{
    wxArrayString names;
    std::vector<player*>& players = currentGameState->get_players();
    std::vector<int> indexMap;

		int mycards;
		for (size_t i = 0; i < players.size(); ++i) {
		        player* p = players[i];
				if (p->get_id() == currentPlayer->get_id()){
						mycards = p->get_nof_cards();
				}
		}
          
    for (size_t i = 0; i < players.size(); ++i) {
        player* p = players[i];
        if (p->get_nof_cards() > (mycards-2)){
        
            continue;

        }
        if (p->get_id() == currentPlayer->get_id()){
            continue;
				}
        names.Add(p->get_player_name());
        indexMap.push_back(i);
    }
		if (names.IsEmpty()) {
        return UINT_MAX;
    }

    int selection = wxGetSingleChoiceIndex(
        "Wähle einen Spieler mit weniger Karten:",
        "EQUALITY",
        names.size(),
        &names[0]
    );

    if (selection == -1)
        return UINT_MAX;

    return indexMap[selection];
}


wxSize MainGamePanel::getBoundsOfRotatedSquare(double edgeLength, double rotationAngle) {
    double newEdgeLength = this->getEdgeLengthOfRotatedSquare(edgeLength, rotationAngle);
    return wxSize(newEdgeLength, newEdgeLength);
}


double MainGamePanel::getEdgeLengthOfRotatedSquare(double originalEdgeLength, double rotationAngle) {
    return originalEdgeLength * (abs(sin(rotationAngle)) + abs(cos(rotationAngle)));
}


wxPoint MainGamePanel::getPointOnEllipse(double horizontalRadius, double verticalRadius, double angle) {
    return wxPoint((int) (sin(angle) * horizontalRadius), (int) (cos(angle) * verticalRadius));
}
