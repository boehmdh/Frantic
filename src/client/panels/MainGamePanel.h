#ifndef FRANTIC_CLIENT_MAINGAMEPANEL_H
#define FRANTIC_CLIENT_MAINGAMEPANEL_H

#include <wx/wx.h>
#include "../../common/game_state/game_state.h"



class MainGamePanel : public wxPanel {

public:
    MainGamePanel(wxWindow* parent);


    void buildGameState(game_state* gameState, player* me);
    uint ShowWishColorMessage();
    uint ShowFantasticFourPlayerMessage();
    uint ShowEqualityPlayerMessage();


private:
    wxBoxSizer* outerLayout = nullptr;
    game_state* currentGameState = nullptr;
    player* currentPlayer = nullptr;
    wxSize la = wxSize(0,32);
    wxFont laF = this->GetFont(); // Large font 32 pixel high
    wxSize lTWin = wxSize(750, 40);
    wxSize sTWin = wxSize(550, 25);
    wxSize labelTWin = wxSize(200,70);

    void buildOtherPlayerHand(game_state* gameState, player* otherPlayer, double playerAngle);
    void buildCardPiles(game_state* gameState, player *me);
    void buildThisPlayer(game_state* gameState, player* me);

    wxSize getBoundsOfRotatedSquare(double edgeLength, double rotationAngle);
    double getEdgeLengthOfRotatedSquare(double originalEdgeLength, double rotationAngle);

    wxPoint getPointOnEllipse(double horizontalRadius, double verticalRadius, double angle);

    wxStaticText* eventBanner = nullptr;
    wxTimer       eventBannerTimer;
    int           eventBannerBlinkCount = 0;

    void StartEventBanner(const wxString& text);
    void OnEventBannerTimer(wxTimerEvent& event);

    // define key layout values
    wxSize panelSize = wxSize(1200, 850);
    wxPoint tableCenter = wxPoint(620, 395);
    wxSize cardSize = wxSize(100, 150);

    double const otherPlayerHandSize = 160.0;
    double const oPlHaDist = 230.0;
    double const oPlLaDist = 380.0;
    double const twoPi = 6.28318530718;
};

#endif //FRANTIC_CLIENT_MAINGAMEPANEL_H
