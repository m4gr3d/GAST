package org.godotengine.plugin.gast.xrapp

import android.os.Bundle
import android.view.View
import androidx.annotation.CallSuper
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import org.godotengine.godot.Godot
import org.godotengine.plugin.vr.openxr.OpenXRPlugin

/**
 * Container fragment.
 *
 * Host and provide Gast related functionality for a derived fragment implementation.
 */
abstract class XrFragment: Fragment(), OpenXRPlugin.EventListener {

    companion object {
        private val TAG = XrFragment::class.java.simpleName
    }

    private val appModel : XrAppModel by activityViewModels()

    @CallSuper
    override fun onViewCreated(view: View, bundle: Bundle?) {
        super.onViewCreated(view, bundle)

        if (appModel.enableXr) {
            appModel.getOpenXRPlugin().registerEventListener(this)
        }
    }

    @CallSuper
    override fun onDestroyView() {
        super.onDestroyView()

        if (appModel.enableXr) {
            appModel.getOpenXRPlugin().unregisterEventListener(this)
        }
    }

    protected fun isXrEnabled() = appModel.enableXr

    /**
     * Enable passthrough
     */
    fun startPassthrough() {
        appModel.startPassthrough(getGodotFragment())
    }

    /**
     * Disable passthrough
     */
    fun stopPassthrough() {
        appModel.stopPassthrough(getGodotFragment())
    }

    private fun getGodotFragment(): Godot? {
        val parentActivity = activity
        if (parentActivity is XrActivity) {
            return parentActivity.godotFragment
        }
        return null
    }

    @CallSuper
    override fun onFocusGained() {
    }

    @CallSuper
    override fun onFocusLost() {
    }

    @CallSuper
    override fun onHeadsetMounted() {
    }

    @CallSuper
    override fun onHeadsetUnmounted() {
    }

    @CallSuper
    override fun onSessionBegun() {
    }

    @CallSuper
    override fun onSessionEnding() {
    }
}
