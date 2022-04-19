package org.godotengine.plugin.gast.samples.edittext

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import org.godotengine.plugin.gast.xrapp.XrFragment
import org.godotengine.plugin.gast.xrapp.view.XrFrameLayout

/**
 * Initialize and render an EditText view in a VR environment.
 */
class EditTextFragment : XrFragment() {

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        bundle: Bundle?
    ): View? {
        return inflater.inflate(R.layout.edittext_layout, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val editTextWrapperView = view.findViewById<XrFrameLayout>(R.id.edittext_wrapper_view)
        editTextWrapperView?.setGazeTracking(true)
    }
}
