package org.godotengine.plugin.gast.view

import android.view.View
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode

/**
 * Keep track of the state of a [GastView] instance.
 */
class GastViewState(val view: View) {
    internal val inputManager = GastViewInputManager(this)
    internal val renderManager = GastViewRenderManager(this)
    internal val children: MutableSet<GastView> = HashSet()

    internal var parent: GastView? = null
    internal var gastNode: GastNode? = null
    internal var gastManager: GastManager? = null
    internal var initialized: Boolean = false
}
