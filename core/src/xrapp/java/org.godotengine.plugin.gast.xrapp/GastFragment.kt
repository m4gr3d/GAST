package org.godotengine.plugin.gast.xrapp

import android.os.Bundle
import android.util.Log
import android.view.View
import android.view.ViewGroup
import androidx.annotation.CallSuper
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import org.godotengine.godot.Godot
import org.godotengine.plugin.gast.view.GastFrameLayout
import org.godotengine.plugin.gast.view.GastView
import org.godotengine.plugin.vr.openxr.OpenXRPlugin

/**
 * Container fragment.
 *
 * Host and provide Gast related functionality for a derived fragment implementation.
 */
abstract class GastFragment: Fragment(), OpenXRPlugin.EventListener {

    companion object {
        private val TAG = GastFragment::class.java.simpleName
    }

    private val appModel : GastAppModel by activityViewModels()
    private var gastView: GastView? = null

    @CallSuper
    override fun onViewCreated(view: View, bundle: Bundle?) {
        super.onViewCreated(view, bundle)

        if (appModel.enableXR) {
            // Wrap the created view is necessary
            if (view !is GastView) {
                val viewParent = view.parent as ViewGroup

                val viewIndex = viewParent.indexOfChild(view)
                viewParent.removeView(view)

                Log.d(TAG, "Generating container view")
                val containerView = GastFrameLayout(requireContext()).apply {
                    addView(view)
                }
                gastView = containerView

                viewParent.addView(containerView, viewIndex)
            } else {
                gastView = view
            }

            appModel.getOpenXRPlugin().registerEventListener(this)
        }
    }

    @CallSuper
    override fun onDestroyView() {
        super.onDestroyView()

        if (appModel.enableXR) {
            appModel.getOpenXRPlugin().unregisterEventListener(this)
        }

        val gastViewRef = getGastView() ?: return
        val gastViewParent = gastViewRef.parent ?: return
        if (gastViewParent is ViewGroup) {
            Log.d(TAG, "Removing container view")
            gastViewParent.removeView(gastViewRef)
        }
    }

    fun getGastView(): View? {
        return gastView?.viewState?.view
    }

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
        if (parentActivity is GastActivity) {
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
