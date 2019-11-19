package com.google.vr.youtube.gast

import android.os.Bundle
import android.support.v4.app.Fragment
import android.support.v4.app.FragmentManager
import android.view.View

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