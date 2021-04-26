package org.godotengine.plugin.gast.samples.edittext

import org.godotengine.godot.FullScreenGodotApp
import org.godotengine.godot.xr.XRMode
import java.util.*

class EditTextActivity : FullScreenGodotApp() {

    override fun initGodotInstance() = EditTextFragment()

    // Override the engine starting parameters to indicate we want VR mode.
    override fun getCommandLine(): List<String> {
        return if (BuildConfig.FLAVOR == "ovr") {
            Collections.singletonList(XRMode.OVR.cmdLineArg)
        } else {
            Collections.emptyList()
        }
    }
}
