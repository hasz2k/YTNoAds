//
//  WebView.swift
//  YTNoAds
//
//  Created by HASZ2Kon 12/11/2026.
//

import SwiftUI
import WebKit

struct WebView: NSViewRepresentable {
    let webView: WKWebView

    func makeNSView(context: Context) -> WKWebView {
        webView
    }

    func updateNSView(_ nsView: WKWebView, context: Context) {}
}
