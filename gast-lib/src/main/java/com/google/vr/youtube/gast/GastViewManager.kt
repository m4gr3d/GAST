package com.google.vr.youtube.gast

import androidx.fragment.app.FragmentManager
import android.os.Bundle
import android.view.View
import androidx.fragment.app.Fragment

class GastViewManager: FragmentManager.FragmentLifecycleCallbacks() {

    override fun onFragmentViewCreated(
        fragmentManager: FragmentManager, fragment: Fragment, view: View, bundle: Bundle?
    ) {
        super.onFragmentViewCreated(fragmentManager, fragment, view, bundle)

    }

    override fun onFragmentViewDestroyed(fragmentManager: FragmentManager, fragment: Fragment) {
        super.onFragmentViewDestroyed(fragmentManager, fragment)
    }
}