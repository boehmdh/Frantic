#include "ImagePanel.h"


ImagePanel::ImagePanel(wxWindow* parent, wxString file, wxBitmapType format, wxPoint position, wxSize size, double rotation) :
        wxPanel(parent, wxID_ANY, position, size)
{
    if(!wxFileExists(file)) {
        wxMessageBox("Could not find file: " + file, "File error", wxICON_ERROR);
        return;
    }

    if(!this->_image.LoadFile(file, format)) {
        wxMessageBox("Could not load file: " + file, "File error", wxICON_ERROR);
        return;
    }

    this->_rotation = rotation;

    this->_width = -1;
    this->_height = -1;

    this->Bind(wxEVT_PAINT, &ImagePanel::paintEvent, this);
    this->Bind(wxEVT_SIZE, &ImagePanel::onSize, this);
}


void ImagePanel::paintEvent(wxPaintEvent& event) {
    // this code is called when the system requests this panel to be redrawn.

    if(!this->_image.IsOk()) {
        return;
    }

    wxPaintDC dc(this);

    int panelW, panelH;
    dc.GetSize(&panelW, &panelH);

    // Only recompute bitmap when size changed
    if (panelW != _width || panelH != _height) {
        wxImage src = _image;

        const int imgW = src.GetWidth();
        const int imgH = src.GetHeight();

        // Preserve aspect ratio
        double scale = std::min(static_cast<double>(panelW) / imgW,static_cast<double>(panelH) / imgH);

        scale = std::min(scale, 1.0);

        int targetW = static_cast<int>(imgW * scale);
        int targetH = static_cast<int>(imgH * scale);

        //resize
        src.Rescale(targetW, targetH, wxIMAGE_QUALITY_HIGH);

        if (_rotation != 0.0) {
            wxPoint center(targetW / 2, targetH / 2);
            src = src.Rotate(_rotation, center, true);
        }

        _bitmap = wxBitmap(src);
        _width  = panelW;
        _height = panelH;
    }

    // Center the bitmap inside the panel
    int bmpW = _bitmap.GetWidth();
    int bmpH = _bitmap.GetHeight();

    int x = (panelW - bmpW) / 2;
    int y = (panelH - bmpH) / 2;

    dc.DrawBitmap(_bitmap, x, y, true);
}


void ImagePanel::onSize(wxSizeEvent& event) {

    // when the user resizes this panel, the image should redraw itself
    Refresh();

    // skip any other effects of this event.
    event.Skip();
}
