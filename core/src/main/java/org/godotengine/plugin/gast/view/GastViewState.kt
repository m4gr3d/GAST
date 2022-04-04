package org.godotengine.plugin.gast.view

import android.view.View
import org.godotengine.plugin.gast.GastManager
import org.godotengine.plugin.gast.GastNode

/**
 * Keep track of the state of a [GastView] instance.
 */
data class GastViewState(
    val view: View,
    var isRoot: Boolean = false,
    var gastNode: GastNode? = null,
    var gastManager: GastManager? = null,
    var initialized: Boolean = false
)
