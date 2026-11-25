#ifndef TEST_CONNECTIONPANEL_H
#define TEST_CONNECTIONPANEL_H

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include "../uiElements/InputField.h"


class ConnectionPanel : public wxPanel {

public:
    ConnectionPanel(wxWindow* parent);

    wxString getServerAddress();
    wxString getServerPort();
    wxString getPlayerName();

private:
    InputField* _serverAddressField;
    InputField* _serverPortField;
    InputField* _playerNameField;
    uint labelWidth = 180;
    //int fPx = 18;
};


#endif //TEST_CONNECTIONPANEL_H
