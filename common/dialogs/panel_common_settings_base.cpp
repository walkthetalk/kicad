///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "panel_common_settings_base.h"

///////////////////////////////////////////////////////////////////////////

PANEL_COMMON_SETTINGS_BASE::PANEL_COMMON_SETTINGS_BASE( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : RESETTABLE_PANEL( parent, id, pos, size, style, name )
{
	wxBoxSizer* bPanelSizer;
	bPanelSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bLeftSizer;
	bLeftSizer = new wxBoxSizer( wxVERTICAL );

	wxGridBagSizer* gbSizer1;
	gbSizer1 = new wxGridBagSizer( 4, 4 );
	gbSizer1->SetFlexibleDirection( wxBOTH );
	gbSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	gbSizer1->SetEmptyCellSize( wxSize( -1,2 ) );

	m_staticTextautosave = new wxStaticText( this, wxID_ANY, _("&Auto save:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextautosave->Wrap( -1 );
	gbSizer1->Add( m_staticTextautosave, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_SaveTime = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	m_SaveTime->SetToolTip( _("Delay after the first change to create a backup file of the board on disk.\nIf set to 0, auto backup is disabled") );

	bSizer6->Add( m_SaveTime, 0, wxALIGN_CENTER_VERTICAL, 5 );

	wxStaticText* minutesLabel;
	minutesLabel = new wxStaticText( this, wxID_ANY, _("minutes"), wxDefaultPosition, wxDefaultSize, 0 );
	minutesLabel->Wrap( -1 );
	bSizer6->Add( minutesLabel, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );


	gbSizer1->Add( bSizer6, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );

	m_staticTextFileHistorySize = new wxStaticText( this, wxID_ANY, _("File history size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFileHistorySize->Wrap( -1 );
	gbSizer1->Add( m_staticTextFileHistorySize, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL, 5 );

	m_fileHistorySize = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 0 );
	gbSizer1->Add( m_fileHistorySize, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_Clear3DCacheFilesOlder = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 120, 30 );
	m_Clear3DCacheFilesOlder->SetToolTip( _("3D cache files older than this are deleted.\nIf set to 0, cache clearing is disabled") );

	bSizer5->Add( m_Clear3DCacheFilesOlder, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_staticTextDays = new wxStaticText( this, wxID_ANY, _("Days"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDays->Wrap( -1 );
	bSizer5->Add( m_staticTextDays, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );


	gbSizer1->Add( bSizer5, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );

	wxStaticText* antialiasingLabel;
	antialiasingLabel = new wxStaticText( this, wxID_ANY, _("Accelerated graphics:"), wxDefaultPosition, wxDefaultSize, 0 );
	antialiasingLabel->Wrap( -1 );
	gbSizer1->Add( antialiasingLabel, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL, 5 );

	wxString m_antialiasingChoices[] = { _("No Antialiasing"), _("Subpixel Antialiasing (High Quality)"), _("Subpixel Antialiasing (Ultra Quality)"), _("Supersampling (2x)"), _("Supersampling (4x)") };
	int m_antialiasingNChoices = sizeof( m_antialiasingChoices ) / sizeof( wxString );
	m_antialiasing = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_antialiasingNChoices, m_antialiasingChoices, 0 );
	m_antialiasing->SetSelection( 0 );
	gbSizer1->Add( m_antialiasing, wxGBPosition( 4, 1 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	wxStaticText* antialiasingLabel1;
	antialiasingLabel1 = new wxStaticText( this, wxID_ANY, _("Fallback graphics:"), wxDefaultPosition, wxDefaultSize, 0 );
	antialiasingLabel1->Wrap( -1 );
	gbSizer1->Add( antialiasingLabel1, wxGBPosition( 6, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL, 5 );

	wxString m_antialiasingFallbackChoices[] = { _("No Antialiasing"), _("Fast Antialiasing"), _("Balanced Antialiasing"), _("High Quality Antialiasing") };
	int m_antialiasingFallbackNChoices = sizeof( m_antialiasingFallbackChoices ) / sizeof( wxString );
	m_antialiasingFallback = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_antialiasingFallbackNChoices, m_antialiasingFallbackChoices, 0 );
	m_antialiasingFallback->SetSelection( 0 );
	gbSizer1->Add( m_antialiasingFallback, wxGBPosition( 6, 1 ), wxGBSpan( 1, 2 ), wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_staticTextClear3DCache = new wxStaticText( this, wxID_ANY, _("3D cache file duration:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextClear3DCache->Wrap( -1 );
	gbSizer1->Add( m_staticTextClear3DCache, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxTOP, 5 );


	gbSizer1->AddGrowableCol( 1 );

	bLeftSizer->Add( gbSizer1, 0, wxEXPAND|wxALL, 10 );

	wxStaticBoxSizer* sizerHelperApps;
	sizerHelperApps = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Helper Applications") ), wxHORIZONTAL );

	wxGridBagSizer* gridHelperApps;
	gridHelperApps = new wxGridBagSizer( 3, 3 );
	gridHelperApps->SetFlexibleDirection( wxBOTH );
	gridHelperApps->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	gridHelperApps->SetEmptyCellSize( wxSize( -1,5 ) );

	wxStaticText* textEditorLabel;
	textEditorLabel = new wxStaticText( sizerHelperApps->GetStaticBox(), wxID_ANY, _("Text editor:"), wxDefaultPosition, wxDefaultSize, 0 );
	textEditorLabel->Wrap( -1 );
	gridHelperApps->Add( textEditorLabel, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL, 4 );

	m_textEditorPath = new wxTextCtrl( sizerHelperApps->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textEditorPath->SetMinSize( wxSize( 280,-1 ) );

	gridHelperApps->Add( m_textEditorPath, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxEXPAND, 8 );

	m_textEditorBtn = new wxBitmapButton( sizerHelperApps->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	gridHelperApps->Add( m_textEditorBtn, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxBOTTOM, 5 );

	m_defaultPDFViewer = new wxRadioButton( sizerHelperApps->GetStaticBox(), wxID_ANY, _("System default PDF viewer"), wxDefaultPosition, wxDefaultSize, 0 );
	gridHelperApps->Add( m_defaultPDFViewer, wxGBPosition( 3, 0 ), wxGBSpan( 1, 3 ), 0, 4 );

	m_otherPDFViewer = new wxRadioButton( sizerHelperApps->GetStaticBox(), wxID_ANY, _("Other:"), wxDefaultPosition, wxDefaultSize, 0 );
	gridHelperApps->Add( m_otherPDFViewer, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL, 4 );

	m_PDFViewerPath = new wxTextCtrl( sizerHelperApps->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_PDFViewerPath->SetMinSize( wxSize( 280,-1 ) );

	gridHelperApps->Add( m_PDFViewerPath, wxGBPosition( 4, 1 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxEXPAND, 8 );

	m_pdfViewerBtn = new wxBitmapButton( sizerHelperApps->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	gridHelperApps->Add( m_pdfViewerBtn, wxGBPosition( 4, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxBOTTOM, 5 );


	gridHelperApps->AddGrowableCol( 1 );

	sizerHelperApps->Add( gridHelperApps, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5 );


	bLeftSizer->Add( sizerHelperApps, 0, wxEXPAND|wxALL, 5 );

	wxStaticBoxSizer* sbSizerIconsOpts;
	sbSizerIconsOpts = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("User Interface") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 3, 3, 0 );
	fgSizer11->AddGrowableCol( 1 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticTexticonscale = new wxStaticText( sbSizerIconsOpts->GetStaticBox(), wxID_ANY, _("Icon scale:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTexticonscale->Wrap( -1 );
	fgSizer11->Add( m_staticTexticonscale, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );

	m_iconScaleSlider = new STEPPED_SLIDER( sbSizerIconsOpts->GetStaticBox(), wxID_ANY, 100, 50, 275, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_VALUE_LABEL );
	m_iconScaleSlider->SetMinSize( wxSize( 140,-1 ) );

	fgSizer11->Add( m_iconScaleSlider, 1, wxBOTTOM|wxEXPAND, 4 );

	m_iconScaleAuto = new wxCheckBox( sbSizerIconsOpts->GetStaticBox(), wxID_ANY, _("Automatic"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_iconScaleAuto, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 25 );

	m_staticTextCanvasScale = new wxStaticText( sbSizerIconsOpts->GetStaticBox(), wxID_ANY, _("Canvas scale:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextCanvasScale->Wrap( -1 );
	fgSizer11->Add( m_staticTextCanvasScale, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_canvasScaleCtrl = new wxSpinCtrlDouble( sbSizerIconsOpts->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0, 1 );
	m_canvasScaleCtrl->SetDigits( 0 );
	fgSizer11->Add( m_canvasScaleCtrl, 0, wxALL|wxEXPAND, 5 );

	m_canvasScaleAuto = new wxCheckBox( sbSizerIconsOpts->GetStaticBox(), wxID_ANY, _("Automatic"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_canvasScaleAuto, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 25 );


	sbSizerIconsOpts->Add( fgSizer11, 0, wxEXPAND|wxRIGHT, 5 );


	sbSizerIconsOpts->Add( 0, 15, 0, wxEXPAND, 5 );

	m_checkBoxIconsInMenus = new wxCheckBox( sbSizerIconsOpts->GetStaticBox(), wxID_ANY, _("Show icons in menus"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerIconsOpts->Add( m_checkBoxIconsInMenus, 0, 0, 4 );


	sbSizerIconsOpts->Add( 0, 0, 1, wxEXPAND, 5 );


	bLeftSizer->Add( sbSizerIconsOpts, 1, wxEXPAND|wxALL, 5 );


	bPanelSizer->Add( bLeftSizer, 1, wxEXPAND|wxBOTTOM, 5 );

	wxBoxSizer* rightSizer;
	rightSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Editing") ), wxVERTICAL );

	m_PreferSelectToDrag = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, _("Prefer selection to dragging"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_PreferSelectToDrag, 0, wxALL, 5 );

	m_warpMouseOnMove = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, _("Warp mouse to origin of moved object"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_warpMouseOnMove, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	m_NonImmediateActions = new wxCheckBox( sbSizer4->GetStaticBox(), wxID_ANY, _("First hotkey selects tool"), wxDefaultPosition, wxDefaultSize, 0 );
	m_NonImmediateActions->SetToolTip( _("If not checked, hotkeys will immediately perform an action even if the relevant tool was not previously selected.") );

	sbSizer4->Add( m_NonImmediateActions, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );


	rightSizer->Add( sbSizer4, 0, wxEXPAND|wxALL, 5 );

	wxStaticBoxSizer* sbSizer41;
	sbSizer41 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Project Backup") ), wxVERTICAL );

	wxGridBagSizer* gbSizer3;
	gbSizer3 = new wxGridBagSizer( 0, 0 );
	gbSizer3->SetFlexibleDirection( wxBOTH );
	gbSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_cbBackupEnabled = new wxCheckBox( sbSizer41->GetStaticBox(), wxID_ANY, _("Automatically backup projects"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbBackupEnabled->SetToolTip( _("Automatically create backup archives of the current project when saving files") );

	gbSizer3->Add( m_cbBackupEnabled, wxGBPosition( 0, 0 ), wxGBSpan( 1, 3 ), wxALL, 5 );

	m_cbBackupAutosave = new wxCheckBox( sbSizer41->GetStaticBox(), wxID_ANY, _("Create backups when auto save occurs"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbBackupAutosave->SetToolTip( _("Create backups when the auto save feature is enabled.  If not checked, backups will only be created when you manually save a file.") );

	gbSizer3->Add( m_cbBackupAutosave, wxGBPosition( 1, 0 ), wxGBSpan( 1, 3 ), wxALL, 5 );

	m_staticText9 = new wxStaticText( sbSizer41->GetStaticBox(), wxID_ANY, _("Maximum backups to keep:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	m_staticText9->SetToolTip( _("How many backup files total to keep (set to 0 for no limit)") );

	gbSizer3->Add( m_staticText9, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_backupLimitTotalFiles = new wxSpinCtrl( sbSizer41->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000, 0 );
	gbSizer3->Add( m_backupLimitTotalFiles, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_staticText10 = new wxStaticText( sbSizer41->GetStaticBox(), wxID_ANY, _("Maximum backups per day:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	m_staticText10->SetToolTip( _("How many backup files to keep each day (set to 0 for no limit)") );

	gbSizer3->Add( m_staticText10, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_backupLimitDailyFiles = new wxSpinCtrl( sbSizer41->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000, 0 );
	gbSizer3->Add( m_backupLimitDailyFiles, wxGBPosition( 3, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_staticText11 = new wxStaticText( sbSizer41->GetStaticBox(), wxID_ANY, _("Minimum time between backups:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	m_staticText11->SetToolTip( _("Number of minutes since the last backup before another will be created the next time you save (set to 0 for no minimum)") );

	gbSizer3->Add( m_staticText11, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_backupMinInterval = new wxSpinCtrl( sbSizer41->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 3600, 0 );
	gbSizer3->Add( m_backupMinInterval, wxGBPosition( 4, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_staticText15 = new wxStaticText( sbSizer41->GetStaticBox(), wxID_ANY, _("minutes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	gbSizer3->Add( m_staticText15, wxGBPosition( 4, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText16 = new wxStaticText( sbSizer41->GetStaticBox(), wxID_ANY, _("Maximum total backup size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	m_staticText16->SetToolTip( _("If the total size of backup files grows above this limit, old backups will be deleted (set to 0 for no limit)") );

	gbSizer3->Add( m_staticText16, wxGBPosition( 5, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_backupLimitTotalSize = new wxSpinCtrl( sbSizer41->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000, 0 );
	gbSizer3->Add( m_backupLimitTotalSize, wxGBPosition( 5, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );

	m_staticText17 = new wxStaticText( sbSizer41->GetStaticBox(), wxID_ANY, _("MB"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	gbSizer3->Add( m_staticText17, wxGBPosition( 5, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	sbSizer41->Add( gbSizer3, 1, wxALL|wxEXPAND, 5 );


	rightSizer->Add( sbSizer41, 1, wxALL|wxEXPAND, 5 );


	bPanelSizer->Add( rightSizer, 0, wxEXPAND|wxALL, 5 );


	this->SetSizer( bPanelSizer );
	this->Layout();

	// Connect Events
	m_textEditorBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_COMMON_SETTINGS_BASE::OnTextEditorClick ), NULL, this );
	m_PDFViewerPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PANEL_COMMON_SETTINGS_BASE::onUpdateUIPdfPath ), NULL, this );
	m_pdfViewerBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_COMMON_SETTINGS_BASE::OnPDFViewerClick ), NULL, this );
	m_pdfViewerBtn->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PANEL_COMMON_SETTINGS_BASE::onUpdateUIPdfPath ), NULL, this );
	m_iconScaleSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleAuto->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PANEL_COMMON_SETTINGS_BASE::OnIconScaleAuto ), NULL, this );
	m_canvasScaleAuto->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PANEL_COMMON_SETTINGS_BASE::OnCanvasScaleAuto ), NULL, this );
}

PANEL_COMMON_SETTINGS_BASE::~PANEL_COMMON_SETTINGS_BASE()
{
	// Disconnect Events
	m_textEditorBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_COMMON_SETTINGS_BASE::OnTextEditorClick ), NULL, this );
	m_PDFViewerPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PANEL_COMMON_SETTINGS_BASE::onUpdateUIPdfPath ), NULL, this );
	m_pdfViewerBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_COMMON_SETTINGS_BASE::OnPDFViewerClick ), NULL, this );
	m_pdfViewerBtn->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PANEL_COMMON_SETTINGS_BASE::onUpdateUIPdfPath ), NULL, this );
	m_iconScaleSlider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( PANEL_COMMON_SETTINGS_BASE::OnScaleSlider ), NULL, this );
	m_iconScaleAuto->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PANEL_COMMON_SETTINGS_BASE::OnIconScaleAuto ), NULL, this );
	m_canvasScaleAuto->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PANEL_COMMON_SETTINGS_BASE::OnCanvasScaleAuto ), NULL, this );

}
