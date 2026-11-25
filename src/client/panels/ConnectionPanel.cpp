#include "ConnectionPanel.h"


#include "../uiElements/ImagePanel.h"
#include "../../common/network/default.conf"
#include "../GameController.h"


ConnectionPanel::ConnectionPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {

    wxColor grey = wxColor(45, 45, 45);
    wxColour lightGrey = wxColour(55, 55, 55);
    this->SetBackgroundColour(grey);

    wxBoxSizer* verticalLayout = new wxBoxSizer(wxVERTICAL);

     //logo
    ImagePanel* logo = new ImagePanel(this, "assets/frantic_logo.png", wxBITMAP_TYPE_ANY, wxDefaultPosition, wxSize(600, 300));
    verticalLayout->Add(logo, 0, wxALIGN_CENTER | wxTOP | wxLEFT | wxRIGHT, 10);

    this->_serverAddressField = new InputField(
        this, // parent element
        "Server address:", // label
        labelWidth, // width of label
        default_server_host, // default value (variable from "default.conf")
        240 // width of field
    );

    this->_serverAddressField->SetBackgroundColour(lightGrey);

    verticalLayout->Add(this->_serverAddressField, 0, wxTOP | wxLEFT | wxRIGHT, 10);

    this->_serverPortField = new InputField(
        this, // parent element
        "Server port:", // label
        labelWidth, // width of label
        wxString::Format("%i", default_port), // default value (variable from "default.conf")
        240 // width of field
    );

    this->_serverPortField->SetBackgroundColour(lightGrey);

    verticalLayout->Add(this->_serverPortField, 0, wxTOP | wxLEFT | wxRIGHT, 10);

    this->_playerNameField = new InputField(
        this, // parent element
        "Player name:", // label
        labelWidth, // width of label
        "", // default value
        240 // width of field
    );

    this->_playerNameField->SetBackgroundColour(lightGrey);

    verticalLayout->Add(this->_playerNameField, 0, wxTOP | wxLEFT | wxRIGHT, 10);

    wxButton* connectButton = new wxButton(this, wxID_ANY, "Connect", wxDefaultPosition, wxSize(120, 20));
    connectButton->SetForegroundColour(*wxWHITE);
    connectButton->SetBackgroundColour(lightGrey);

    connectButton->Bind(wxEVT_BUTTON, [](wxCommandEvent& event) {
        GameController::connectToServer();
    });
    verticalLayout->Add(connectButton, 0, wxALIGN_RIGHT | wxALL, 10);

    this->SetSizerAndFit(verticalLayout);
}


wxString ConnectionPanel::getServerAddress() {
    return this->_serverAddressField->getValue();
}


wxString ConnectionPanel::getServerPort() {
    return this->_serverPortField->getValue();
}


wxString ConnectionPanel::getPlayerName() {
    return this->_playerNameField->getValue();
}
