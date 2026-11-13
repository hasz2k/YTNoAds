//
//  BrowserModel.swift
//  YTNoAds
//
//  Created by HASZ2K on 12/11/2026.
//

import AppKit
import Combine
import OSLog
import WebKit

@MainActor
final class BrowserModel: NSObject, ObservableObject {
    @Published private(set) var canGoBack = false
    @Published private(set) var canGoForward = false
    @Published private(set) var pageTitle = "YTNoAds"
    @Published private(set) var estimatedProgress: Double = 0

    let webView: WKWebView
    private let logger = Logger(subsystem: AppConfig.logSubsystem, category: "browser")

    override init() {
        webView = Self.makeWebView()
        super.init()

        webView.uiDelegate = self
        webView.navigationDelegate = self
        bindNavigationState()

        Task {
            await installContentRules()
            goHome()
        }
    }

    func goHome() { webView.load(URLRequest(url: AppConfig.homeURL)) }
    func goBack() { webView.goBack() }
    func goForward() { webView.goForward() }
    func reload() { webView.reload() }

    func openInDefaultBrowser() {
        guard let url = webView.url else { return }
        NSWorkspace.shared.open(url)
    }

    private static func makeWebView() -> WKWebView {
        let configuration = WKWebViewConfiguration()
        configuration.mediaTypesRequiringUserActionForPlayback = []
        configuration.preferences.isElementFullscreenEnabled = true
        configuration.websiteDataStore = .default()
        configuration.userContentController.addUserScript(
            WKUserScript(
                source: AdSkipScript.source,
                injectionTime: .atDocumentStart,
                forMainFrameOnly: false
            )
        )

        let webView = WKWebView(frame: .zero, configuration: configuration)
        webView.customUserAgent = AppConfig.userAgent
        webView.allowsBackForwardNavigationGestures = true
        return webView
    }

    private func bindNavigationState() {
        webView.publisher(for: \.canGoBack)
            .receive(on: DispatchQueue.main)
            .assign(to: &$canGoBack)

        webView.publisher(for: \.canGoForward)
            .receive(on: DispatchQueue.main)
            .assign(to: &$canGoForward)

        webView.publisher(for: \.title)
            .compactMap { title in
                guard let title, !title.isEmpty else { return nil }
                return title
            }
            .receive(on: DispatchQueue.main)
            .assign(to: &$pageTitle)

        webView.publisher(for: \.estimatedProgress)
            .receive(on: DispatchQueue.main)
            .assign(to: &$estimatedProgress)
    }

    private func installContentRules() async {
        do {
            let json = try AdBlockRules.encodedJSON()
            guard let ruleList = try await WKContentRuleListStore.default().compileContentRuleList(
                forIdentifier: AppConfig.ruleListIdentifier,
                encodedContentRuleList: json
            ) else { return }

            webView.configuration.userContentController.add(ruleList)
        } catch {
            logger.error("Content rule compilation failed: \(error.localizedDescription)")
        }
    }
}

extension BrowserModel: WKUIDelegate {
    func webView(
        _ webView: WKWebView,
        createWebViewWith configuration: WKWebViewConfiguration,
        for navigationAction: WKNavigationAction,
        windowFeatures: WKWindowFeatures
    ) -> WKWebView? {
        if navigationAction.targetFrame == nil {
            webView.load(navigationAction.request)
        }
        return nil
    }
}

extension BrowserModel: WKNavigationDelegate {
    func webView(
        _ webView: WKWebView,
        decidePolicyFor navigationAction: WKNavigationAction
    ) async -> WKNavigationActionPolicy {
        guard navigationAction.navigationType == .linkActivated,
              let url = navigationAction.request.url,
              !AppConfig.isInternal(url)
        else {
            return .allow
        }

        NSWorkspace.shared.open(url)
        return .cancel
    }
}
