package org.godotengine.plugin.gast.activity

import android.content.ComponentName
import android.content.Intent
import android.content.pm.ActivityInfo
import android.content.res.Configuration
import android.os.Bundle
import android.util.Log
import androidx.annotation.CallSuper
import androidx.annotation.LayoutRes
import androidx.fragment.app.FragmentActivity
import org.godotengine.godot.Godot
import org.godotengine.godot.GodotHost
import org.godotengine.godot.GodotInstrumentation
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode
import org.godotengine.plugin.gast.R
import org.godotengine.plugin.gast.view.GastFrameLayout

abstract class GastActivity : FragmentActivity(), GodotHost {

    companion object {
        private val TAG = GastActivity::class.java.simpleName
    }

    private var godotFragment: Godot? = null
    private var gastFrameLayout: GastFrameLayout? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        Log.d(TAG, "ON CREATE")
        super.onCreate(savedInstanceState)

        val currentFragment =
            supportFragmentManager.findFragmentById(R.id.godot_fragment_container)

        if (inSpatialMode()) {
            super.setContentView(R.layout.godot_app_layout)

            if (currentFragment is Godot) {
                Log.v(TAG, "Reusing existing Godot fragment instance.")
                godotFragment = currentFragment
            } else {
                Log.v(TAG, "Creating new Godot fragment instance.")
                godotFragment = initGodotInstance()
                checkNotNull(godotFragment) { "Godot instance must be non-null." }
                supportFragmentManager
                    .beginTransaction()
                    .replace(R.id.godot_fragment_container, godotFragment!!)
                    .setPrimaryNavigationFragment(godotFragment)
                    .commitNowAllowingStateLoss()
            }
        } else {
            godotFragment = null
        }
    }

    protected fun inSpatialMode() = resources.configuration.orientation == Configuration.ORIENTATION_LANDSCAPE


    override fun setContentView(@LayoutRes layoutResID: Int) {
        if (inSpatialMode()) {
            if (godotFragment == null) {
                throw IllegalStateException("setContentView(...) must be called after super" +
                    ".onCreate(...)")
            }

            // Add this point, Godot::onCreate(...) should have already been called giving us access
            // the registered plugins.
            val gastPlugin = getGastManager()
            val containerView = gastPlugin.rootView

            // Any view we want to render in VR needs to be added as a child to the containerView in
            // order to be hooked to the view lifecycle events.
            val contentView = layoutInflater.inflate(layoutResID, containerView, false)
            gastFrameLayout = if (contentView is GastFrameLayout) {
                contentView
            } else {
                // Wrap the content view in a GastFrameLayout view.
                GastFrameLayout(this).apply {
                    addView(contentView)
                }
            }
            containerView.addView(gastFrameLayout)
        } else {
            super.setContentView(layoutResID)
        }
    }

//    override fun onStop() {
//        Log.d(TAG, "ON DESTROY")
//        // Remove the Godot fragment
//        if (godotFragment != null) {
//            Log.d(TAG, "Removing godot instance")
//            supportFragmentManager
//                .beginTransaction()
//                .remove(godotFragment!!)
//                .commitNowAllowingStateLoss()
//        }
//
//        super.onStop()
//    }

    override fun onGodotMainLoopStarted() {
        // Complete setup and initializing of the GastFrameLayout
        super.onGodotMainLoopStarted()

        val gastNode = GastNode(getGastManager(), "GastActivityContainer")

        runOnUiThread {
            gastFrameLayout?.initialize(getGastManager(), gastNode)
        }
    }

    private fun getGastManager(): GastManager {
        val gastPlugin = GodotPluginRegistry.getPluginRegistry().getPlugin("gast-core")
        if (gastPlugin !is GastManager) throw IllegalStateException("Unable to retrieve the Gast plugin.")

        return gastPlugin
    }

    // Override the engine starting parameters to indicate we want VR mode.
//    override fun getCommandLine(): List<String> {
//        return if (inSpatialMode()) {
//            Collections.singletonList(XRMode.OVR.cmdLineArg)
//        } else {
//            Collections.emptyList()
//        }
//    }

    override fun onGodotRestartRequested(instance: Godot) {
        if (instance === godotFragment) {
            // HACK:
            //
            // Currently it's very hard to properly deinitialize Godot on Android to restart the game
            // from scratch. Therefore, we need to kill the whole app process and relaunch it.
            //
            // Restarting only the activity, wouldn't be enough unless it did proper cleanup (including
            // releasing and reloading native libs or resetting their state somehow and clearing statics).
            //
            // Using instrumentation is a way of making the whole app process restart, because Android
            // will kill any process of the same package which was already running.
            //
            val args = Bundle()
            args.putParcelable("intent", intent)
            startInstrumentation(ComponentName(this, GodotInstrumentation::class.java), null, args)
        }
    }

    override fun onNewIntent(intent: Intent?) {
        super.onNewIntent(intent)
        if (godotFragment != null) {
            godotFragment!!.onNewIntent(intent)
        }
    }

    @CallSuper
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (godotFragment != null) {
            godotFragment!!.onActivityResult(requestCode, resultCode, data)
        }
    }

    @CallSuper
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String?>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (godotFragment != null) {
            godotFragment!!.onRequestPermissionsResult(requestCode, permissions, grantResults)
        }
    }

    override fun onBackPressed() {
        if (godotFragment != null) {
            godotFragment!!.onBackPressed()
        } else {
            super.onBackPressed()
        }
    }

    /**
     * Used to initialize the Godot fragment instance in [onCreate].
     */
    protected fun initGodotInstance(): Godot {
        return GastGodot()
    }

    protected fun getGodotFragment(): Godot? {
        return godotFragment
    }

}
