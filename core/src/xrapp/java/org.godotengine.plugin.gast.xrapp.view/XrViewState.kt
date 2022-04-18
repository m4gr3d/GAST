package org.godotengine.plugin.gast.xrapp.view

import android.view.View
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode

/**
 * Keep track of the state of a [XrView] instance.
 */
class XrViewState(val view: View) {
    internal val inputManager = XrViewInputManager(this)
    internal val renderManager = XrViewRenderManager(this)
    internal val children: MutableSet<XrView> = HashSet()

    internal var parent: XrView? = null
    internal var gastNode: GastNode? = null
    internal var gastManager: GastManager? = null
    internal var initialized: Boolean = false
}
