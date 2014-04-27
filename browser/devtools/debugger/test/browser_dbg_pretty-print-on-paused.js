/* Any copyright is dedicated to the Public Domain.
  http://creativecommons.org/publicdomain/zero/1.0/ */

/**
 * Test that pretty printing when the debugger is paused does not switch away
 * from the selected source.
 */

const TAB_URL = EXAMPLE_URL + "doc_pretty-print-on-paused.html";

let gTab, gDebuggee, gPanel, gDebugger, gThreadClient, gSources;

const SECOND_SOURCE_VALUE = EXAMPLE_URL + "code_ugly-2.js";

function test(){
  initDebugger(TAB_URL).then(([aTab, aDebuggee, aPanel]) => {
    gTab = aTab;
    gDebuggee = aDebuggee;
    gPanel = aPanel;
    gDebugger = gPanel.panelWin;
    gThreadClient = gDebugger.gThreadClient;
    gSources = gDebugger.DebuggerView.Sources;

    Task.spawn(function* () {
      try {
        yield ensureSourceIs(gPanel, "code_script-switching-02.js", true);

        yield doInterrupt(gPanel);
        yield rdpInvoke(gThreadClient, gThreadClient.setBreakpoint, {
          url: gSources.selectedValue,
          line: 6
        });
        yield doResume(gPanel);

        const bpHit = waitForCaretAndScopes(gPanel, 6);
        // Get the debuggee call off this tick so that we aren't accidentally
        // blocking the yielding of bpHit which causes a deadlock.
        executeSoon(() => gDebuggee.secondCall());
        yield bpHit;

        info("Switch to the second source.");
        const sourceShown = waitForSourceShown(gPanel, SECOND_SOURCE_VALUE);
        gSources.selectedValue = SECOND_SOURCE_VALUE;
        yield sourceShown;

        info("Pretty print the source.");
        const prettyPrinted = waitForSourceShown(gPanel, SECOND_SOURCE_VALUE);
        gDebugger.document.getElementById("pretty-print").click();
        yield prettyPrinted;

        yield resumeDebuggerThenCloseAndFinish(gPanel);
      } catch (e) {
        DevToolsUtils.reportException("browser_dbg_pretty-print-on-paused.js", e);
        ok(false, "Got an error: " + DevToolsUtils.safeErrorString(e));
      }
    });
  });
}

registerCleanupFunction(function() {
  gTab = null;
  gDebuggee = null;
  gPanel = null;
  gDebugger = null;
  gThreadClient = null;
  gSources = null;
});
