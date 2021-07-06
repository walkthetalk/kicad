/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 CERN
 * Copyright (C) 2017-2021 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * @author Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 * @author Maciej Suminski <maciej.suminski@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * https://www.gnu.org/licenses/gpl-3.0.html
 * or you may search the http://www.gnu.org website for the version 3 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef __SIM_PLOT_FRAME__
#define __SIM_PLOT_FRAME__

/**
 * @file sim_plot_frame.h
 *
 * Subclass of SIM_PLOT_FRAME_BASE, which is generated by wxFormBuilder.
 */

#include "sim_plot_frame_base.h"
#include "sim_types.h"

#include <kiway_player.h>
#include <dialogs/dialog_sim_settings.h>

#include <wx/event.h>

#include <list>
#include <memory>
#include <map>

class SCH_EDIT_FRAME;
class SCH_SYMBOL;

class SPICE_SIMULATOR;
class SPICE_SIMULATOR_SETTINGS;
class NETLIST_EXPORTER_PSPICE_SIM;

#include "sim_plot_panel.h"
#include "sim_panel_base.h"
#include "sim_workbook.h"

class SIM_THREAD_REPORTER;
class TUNER_SLIDER;

/**
 * Implementing SIM_PLOT_FRAME_BASE
 */
class SIM_PLOT_FRAME : public SIM_PLOT_FRAME_BASE
{
public:
    SIM_PLOT_FRAME( KIWAY* aKiway, wxWindow* aParent );
    ~SIM_PLOT_FRAME();

    void StartSimulation( const wxString& aSimCommand = wxEmptyString );
    void StopSimulation();

    /**
     * Create a new plot panel for a given simulation type and adds it to the main notebook.
     *
     * @param aSimType is requested simulation type.
     * @return The new plot panel.
     */
    SIM_PANEL_BASE* NewPlotPanel( wxString aSimCommand );

    /**
     * Add a voltage plot for a given net name.
     *
     * @param aNetName is the net name for which a voltage plot should be created.
     */
    void AddVoltagePlot( const wxString& aNetName );

    /**
     * Add a current plot for a particular device.
     *
     * @param aDeviceName is the device name (e.g. R1, C1).
     * @param aParam is the current type (e.g. I, Ic, Id).
     */
    void AddCurrentPlot( const wxString& aDeviceName, const wxString& aParam );

    /**
     * Add a tuner for a symbol.
     */
    void AddTuner( SCH_SYMBOL* aSymbol );

    /**
     * Remove an existing tuner.
     *
     * @param aTuner is the tuner to be removed.
     * @param aErase decides whether the tuner should be also removed from the tuners list.
     * Otherwise it is removed only from the SIM_PLOT_FRAME pane.
     */
    void RemoveTuner( TUNER_SLIDER* aTuner, bool aErase = true );

    /**
     * Return the currently opened plot panel (or NULL if there is none).
     */
    SIM_PLOT_PANEL* CurrentPlot() const;

    /**
     * Return the netlist exporter object used for simulations.
     */
    const NETLIST_EXPORTER_PSPICE_SIM* GetExporter() const;

    /**
     * @return the current background option for plotting.
     * false for drak bg, true for clear bg
     */
    bool GetPlotBgOpt() const { return m_plotUseWhiteBg; }

    void LoadSettings( APP_SETTINGS_BASE* aCfg ) override;

    void SaveSettings( APP_SETTINGS_BASE* aCfg ) override;

    WINDOW_SETTINGS* GetWindowSettings( APP_SETTINGS_BASE* aCfg ) override;

    // Simulator doesn't host a tool framework
    wxWindow* GetToolCanvas() const override { return nullptr; }

    std::shared_ptr<SPICE_SIMULATOR_SETTINGS>& GetSimulatorSettings();

private:
    /**
     * Load the currently active workbook stored in the project settings. If there is none,
     * generate a filename for the currently active workbook and store it in the project settings.
     */
    void initWorkbook();

    /**
     * Set the main window title bar text.
     */
    void updateTitle();

    /**
     * Update the frame to match the changes to the workbook. Should be always called after the
     * workbook was modified.
     */
    void updateFrame();

    /**
     * Give icons to menuitems of the main menubar.
     */
    void setIconsForMenuItems();

    /**
     * Return the currently opened plot panel (or NULL if there is none).
     */
    SIM_PANEL_BASE* currentPlotWindow() const
    {
        return dynamic_cast<SIM_PANEL_BASE*>( m_workbook->GetCurrentPage() );
    }

    /**
     * Add a new plot to the current panel.
     *
     * @param aName is the device/net name.
     * @param aType describes the type of plot.
     * @param aParam is the parameter for the device/net (e.g. I, Id, V).
     */
    void addPlot( const wxString& aName, SIM_PLOT_TYPE aType, const wxString& aParam );

    /**
     * Remove a plot with a specific title.
     *
     * @param aPlotName is the full plot title (e.g. I(Net-C1-Pad1)).
     */
    void removePlot( const wxString& aPlotName );

    /**
     * Reload the current schematic for the netlist exporter.
     */
    void updateNetlistExporter();

    /**
     * Update plot in a particular SIM_PLOT_PANEL. If the panel does not contain
     * the plot, it will be added.
     *
     * @param aDescriptor contains the plot description.
     * @param aPanel is the panel that should receive the update.
     * @return True if a plot was successfully added/updated.
     */
    bool updatePlot( const wxString& aName, SIM_PLOT_TYPE aType, const wxString& aParam,
                     SIM_PLOT_PANEL* aPlotPanel );

    /**
     * Update the list of currently plotted signals.
     */
    void updateSignalList();

    /**
     * Filter out tuners for components that do not exist anymore.
     *
     * Decisions are based on the current #NETLIST_EXPORTER_BASE data.
     */
    void updateTuners();

    /**
     * Apply component values specified using tuner sliders to the current netlist.
     */
    void applyTuners();

    /**
     * Load plot settings from a file.
     *
     * @param aPath is the file name.
     * @return True if successful.
     */
    bool loadWorkbook( const wxString& aPath );

    /**
     * Save plot settings to a file.
     *
     * @param aPath is the file name.
     * @return True if successful.
     */
    bool saveWorkbook( const wxString& aPath );

    /**
     * Return X axis for a given simulation type.
     */
    SIM_PLOT_TYPE GetXAxisType( SIM_TYPE aType ) const;

    // Menu handlers
    void menuNewPlot( wxCommandEvent& aEvent ) override;
    void menuOpenWorkbook( wxCommandEvent& event ) override;
    void menuSaveWorkbook( wxCommandEvent& event ) override;
    void menuSaveWorkbookAs( wxCommandEvent& event ) override;

    void menuExit( wxCommandEvent& event ) override
    {
        Close();
    }

    void menuSaveImage( wxCommandEvent& event ) override;
    void menuSaveCsv( wxCommandEvent& event ) override;
    void menuZoomIn( wxCommandEvent& event ) override;
    void menuZoomOut( wxCommandEvent& event ) override;
    void menuZoomFit( wxCommandEvent& event ) override;
    void menuShowGrid( wxCommandEvent& event ) override;
    void menuShowGridUpdate( wxUpdateUIEvent& event ) override;
    void menuShowLegend( wxCommandEvent& event ) override;
    void menuShowLegendUpdate( wxUpdateUIEvent& event ) override;
    void menuShowDotted( wxCommandEvent& event ) override;
    void menuShowDottedUpdate( wxUpdateUIEvent& event ) override;
	void menuWhiteBackground( wxCommandEvent& event ) override;
	void menuShowWhiteBackgroundUpdate( wxUpdateUIEvent& event ) override
    {
        event.Check( m_plotUseWhiteBg );
    }

    // Event handlers
    void onPlotClose( wxAuiNotebookEvent& event ) override;
    void onPlotClosed( wxAuiNotebookEvent& event ) override;
    void onPlotChanged( wxAuiNotebookEvent& event ) override;
    void onPlotDragged( wxAuiNotebookEvent& event ) override;

    void onSignalDblClick( wxMouseEvent& event ) override;
    void onSignalRClick( wxListEvent& event ) override;

    void onSimulate( wxCommandEvent& event );
    void onSettings( wxCommandEvent& event );
    void onAddSignal( wxCommandEvent& event );
    void onProbe( wxCommandEvent& event );
    void onTune( wxCommandEvent& event );
    void onShowNetlist( wxCommandEvent& event );

    bool canCloseWindow( wxCloseEvent& aEvent ) override;
    void doCloseWindow() override;

    void onCursorUpdate( wxCommandEvent& aEvent );
    void onSimUpdate( wxCommandEvent& aEvent );
    void onSimReport( wxCommandEvent& aEvent );
    void onSimStarted( wxCommandEvent& aEvent );
    void onSimFinished( wxCommandEvent& aEvent );

    // adjust the sash dimension of splitter windows after reading
    // the config settings
    // must be called after the config settings are read, and once the
    // frame is initialized (end of the Ctor)
    void setSubWindowsSashSize();

    // Toolbar buttons
    wxToolBarToolBase* m_toolSimulate;
    wxToolBarToolBase* m_toolAddSignals;
    wxToolBarToolBase* m_toolProbe;
    wxToolBarToolBase* m_toolTune;
    wxToolBarToolBase* m_toolSettings;

    SCH_EDIT_FRAME* m_schematicFrame;
    std::unique_ptr<NETLIST_EXPORTER_PSPICE_SIM> m_exporter;
    std::shared_ptr<SPICE_SIMULATOR> m_simulator;
    SIM_THREAD_REPORTER* m_reporter;

    ///< List of currently displayed tuners
    std::list<TUNER_SLIDER*> m_tuners;

    // Trick to preserve settings between runs:
    // the DIALOG_SIM_SETTINGS is not destroyed after closing the dialog.
    // Once created it will be not shown (shown only on request) during a session
    // and will be destroyed only when closing the simulator frame.
    DIALOG_SIM_SETTINGS* m_settingsDlg;

    // Right click context menu for signals in the listbox
    class SIGNAL_CONTEXT_MENU : public wxMenu
    {
        public:
            SIGNAL_CONTEXT_MENU( const wxString& aSignal, SIM_PLOT_FRAME* aPlotFrame );

        private:
            void onMenuEvent( wxMenuEvent& aEvent );

            const wxString& m_signal;
            SIM_PLOT_FRAME* m_plotFrame;

            enum SIGNAL_CONTEXT_MENU_EVENTS
            {
                HIDE_SIGNAL,
                SHOW_CURSOR,
                HIDE_CURSOR
            };
    };

    ///< Panel that was used as the most recent one for simulations
    SIM_PLOT_PANEL* m_lastSimPlot;

    ///< imagelists used to add a small colored icon to signal names
    ///< and cursors name, the same color as the corresponding signal traces
    wxImageList* m_signalsIconColorList;

    ///< A string to store the path of saved workbooks during a session
    static wxString m_savedWorkbooksPath;

    // Variables for temporary storage:
    int m_splitterLeftRightSashPosition;
    int m_splitterPlotAndConsoleSashPosition;
    int m_splitterSignalsSashPosition;
    int m_splitterTuneValuesSashPosition;
    bool m_plotUseWhiteBg;
    unsigned int m_plotNumber;
};

// Commands
wxDECLARE_EVENT( EVT_SIM_UPDATE, wxCommandEvent );
wxDECLARE_EVENT( EVT_SIM_REPORT, wxCommandEvent );

// Notifications
wxDECLARE_EVENT( EVT_SIM_STARTED, wxCommandEvent );
wxDECLARE_EVENT( EVT_SIM_FINISHED, wxCommandEvent );

#endif // __sim_plot_frame__
