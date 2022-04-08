package org.godotengine.plugin.gast.samples.edittext

import android.os.Bundle
import org.godotengine.godot.FullScreenGodotApp
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.godot.xr.XRMode
import org.godotengine.plugin.vr.oculus.mobile.OvrMobilePlugin
import java.util.*

class EditTextActivity : FullScreenGodotApp(), OvrMobilePlugin.OvrEventListener {

    override fun initGodotInstance() = EditTextFragment()

    override fun onCreate(bundle: Bundle?) {
        super.onCreate(bundle)
        getOvrMobilePlugin().registerOvrEventListener(this)
    }

    override fun onDestroy() {
        getOvrMobilePlugin().unregisterOvrEventListener(this)
        super.onDestroy()
    }

    private fun getOvrMobilePlugin(): OvrMobilePlugin {
        val ovrPlugin = GodotPluginRegistry.getPluginRegistry().getPlugin("OVRMobile")
        if (ovrPlugin !is OvrMobilePlugin) {
            throw IllegalStateException("Unable to retrieve OVR Mobile plugin.")
        }

        return ovrPlugin
    }

    // Override the engine starting parameters to indicate we want VR mode.
    override fun getCommandLine(): List<String> {
        return if (BuildConfig.FLAVOR == "ovr") {
            Collections.singletonList(XRMode.OVR.cmdLineArg)
        } else {
            Collections.emptyList()
        }
    }

    override fun onEnterVrMode() {
    }

    override fun onHeadsetMounted() {
    }

    override fun onHeadsetUnmounted() {
    }

    override fun onInputFocusGained() {
    }

    override fun onInputFocusLost() {
    }

    override fun onLeaveVrMode() {
    }

    override fun onPoseRecentered() {
    }
}
