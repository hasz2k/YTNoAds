//
//  ContentView.swift
//  YTNoAds
//
//  Created by HASZ2K on 12/11/2026.
//

import SwiftUI

struct ContentView: View {
    @StateObject private var browser = BrowserModel()

    var body: some View {
        WebView(webView: browser.webView)
            .frame(
                minWidth: AppConfig.minimumWindowSize.width,
                minHeight: AppConfig.minimumWindowSize.height
            )
            .overlay(alignment: .top) {
                loadingIndicator
            }
            .navigationTitle(browser.pageTitle)
            .toolbar {
                navigationToolbar
            }
    }

    @ViewBuilder
    private var loadingIndicator: some View {
        if browser.estimatedProgress > 0 && browser.estimatedProgress < 1 {
            ProgressView(value: browser.estimatedProgress)
                .progressViewStyle(.linear)
                .tint(.red)
        }
    }

    @ToolbarContentBuilder
    private var navigationToolbar: some ToolbarContent {
        ToolbarItemGroup {
            Button(action: browser.goBack) {
                Label("Wstecz", systemImage: "chevron.left")
            }
            .keyboardShortcut("[", modifiers: .command)
            .disabled(!browser.canGoBack)

            Button(action: browser.goForward) {
                Label("Dalej", systemImage: "chevron.right")
            }
            .keyboardShortcut("]", modifiers: .command)
            .disabled(!browser.canGoForward)

            Button(action: browser.reload) {
                Label("Odśwież", systemImage: "arrow.clockwise")
            }
            .keyboardShortcut("r", modifiers: .command)

            Button(action: browser.goHome) {
                Label("Strona główna", systemImage: "house")
            }
            .keyboardShortcut("h", modifiers: [.command, .shift])

            Button(action: browser.openInDefaultBrowser) {
                Label("Otwórz w przeglądarce", systemImage: "safari")
            }
            .keyboardShortcut("o", modifiers: [.command, .shift])
        }
    }
}
