#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <glib/gstdio.h>
#include <string.h>

#define APP_NAME     "YTNoAds"
#define APP_ID       "pl.ytnoads.linux"
#define HOME_URL     "https://www.youtube.com"
#define FILTER_ID    "ytnoads-rules"
#define APP_DATA_KEY "app-widgets"
#define USER_AGENT   \
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/605.1.15 (KHTML, like Gecko) " \
    "Version/17.4 Safari/605.1.15"

    static const char *const ADBLOCK_RULES_JSON =
    "["
    "  {\"trigger\": {\"url-filter\": \"doubleclick\\\\.net\"}, \"action\": {\"type\": \"block\"}},"
    "  {\"trigger\": {\"url-filter\": \"googlesyndication\\\\.com\"}, \"action\": {\"type\": \"block\"}},"
    "  {\"trigger\": {\"url-filter\": \"googleadservices\\\\.com\"}, \"action\": {\"type\": \"block\"}},"
    "  {\"trigger\": {\"url-filter\": \"youtube\\\\.com/pagead/\"}, \"action\": {\"type\": \"block\"}},"
    "  {\"trigger\": {\"url-filter\": \"youtube\\\\.com/api/stats/ads\"}, \"action\": {\"type\": \"block\"}},"
    "  {\"trigger\": {\"url-filter\": \"youtube\\\\.com/ptracking\"}, \"action\": {\"type\": \"block\"}},"
    "  {\"trigger\": {\"url-filter\": \"youtube\\\\.com/get_midroll\"}, \"action\": {\"type\": \"block\"}},"
    "  {\"trigger\": {\"url-filter\": \"youtube\\\\.com/youtubei/v1/player/ad_break\"}, \"action\": {\"type\": \"block\"}}"
    "]";

    static const char *const ADSKIP_SCRIPT_JS =
    "(() => {"
    "  'use strict';"
    "  const HIDDEN_SELECTORS = ["
    "    '.ytp-ad-overlay-container', '.ytp-ad-text-overlay', '.ytp-ad-image-overlay',"
    "    'ytd-ad-slot-renderer', 'ytd-in-feed-ad-layout-renderer', 'ytd-display-ad-renderer',"
    "    'ytd-promoted-sparkles-web-renderer', 'ytd-banner-promo-renderer',"
    "    'ytd-companion-slot-renderer', 'ytd-player-legacy-desktop-watch-ads-renderer',"
    "    '#masthead-ad', '#player-ads', '#panels ytd-ads-engagement-panel-content-renderer'"
    "  ];"
    "  const SKIP_SELECTORS = ["
    "    '.ytp-ad-skip-button', '.ytp-ad-skip-button-modern', '.ytp-skip-ad-button',"
    "    '.ytp-ad-skip-button-container button'"
    "  ];"
    "  const injectStyle = () => {"
    "    if (document.getElementById('ytnoads-style')) return;"
    "    const style = document.createElement('style');"
    "    style.id = 'ytnoads-style';"
    "    style.textContent = `${HIDDEN_SELECTORS.join(',')} { display: none !important; }`;"
    "    (document.head || document.documentElement).appendChild(style);"
    "  };"
    "  let mutedByScript = false;"
    "  const fastForwardAd = (video) => {"
    "    if (!video.muted) { video.muted = true; mutedByScript = true; }"
    "    video.playbackRate = 16;"
    "    if (Number.isFinite(video.duration) && video.duration > 0) {"
    "      video.currentTime = video.duration;"
    "    }"
    "    document.querySelectorAll(SKIP_SELECTORS.join(',')).forEach((b) => b.click());"
    "  };"
    "  const restorePlayback = (video) => {"
    "    if (!mutedByScript) return;"
    "    video.muted = false;"
    "    video.playbackRate = 1;"
    "    mutedByScript = false;"
    "  };"
    "  setInterval(() => {"
    "    injectStyle();"
    "    const player = document.querySelector('.html5-video-player');"
    "    const video = document.querySelector('video.html5-main-video') || document.querySelector('video');"
    "    if (!player || !video) return;"
    "    if (player.classList.contains('ad-showing') || player.classList.contains('ad-interrupting')) {"
    "      fastForwardAd(video);"
    "    } else {"
    "      restorePlayback(video);"
    "    }"
    "  }, 150);"
    "})();";

static const char *const INTERNAL_DOMAINS[] = {
    "youtube.com", "google.com", "gstatic.com", "ytimg.com", "googlevideo.com",
};

typedef struct {
    GtkWindow     *window;
    WebKitWebView *web_view;
    GtkWidget     *header;
    GtkWidget     *back_btn;
    GtkWidget     *forward_btn;
    GtkWidget     *progress_bar;
} AppWidgets;

static gboolean host_matches_domain(const char *host, const char *domain) {
    if (g_ascii_strcasecmp(host, domain) == 0)
        return TRUE;

    gsize host_len = strlen(host);
    gsize domain_len = strlen(domain);

    return host_len > domain_len + 1 &&
           host[host_len - domain_len - 1] == '.' &&
           g_ascii_strcasecmp(host + host_len - domain_len, domain) == 0;
}

static gboolean is_internal_uri(const char *uri_str) {
    if (!uri_str)
        return FALSE;

    g_autoptr(GUri) uri = g_uri_parse(uri_str, G_URI_FLAGS_NONE, NULL);
    const char *host = uri ? g_uri_get_host(uri) : NULL;
    if (!host)
        return FALSE;

    for (gsize i = 0; i < G_N_ELEMENTS(INTERNAL_DOMAINS); i++) {
        if (host_matches_domain(host, INTERNAL_DOMAINS[i]))
            return TRUE;
    }
    return FALSE;
}

static void on_launch_finished(GObject *source, GAsyncResult *res, gpointer user_data G_GNUC_UNUSED) {
    g_autoptr(GError) error = NULL;
    if (!gtk_uri_launcher_launch_finish(GTK_URI_LAUNCHER(source), res, &error) &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
        g_warning("Nie udało się otworzyć adresu: %s", error->message);
    }
}

static void open_external_uri(GtkWindow *parent, const char *uri) {
    if (!uri)
        return;

    g_autoptr(GtkUriLauncher) launcher = gtk_uri_launcher_new(uri);
    gtk_uri_launcher_launch(launcher, parent, NULL, on_launch_finished, NULL);
}

static void on_title_changed(GObject *object G_GNUC_UNUSED, GParamSpec *pspec G_GNUC_UNUSED,
                             AppWidgets *app) {
    const char *title = webkit_web_view_get_title(app->web_view);
    gtk_window_set_title(app->window, (title && *title) ? title : APP_NAME);
}

static void on_load_changed(WebKitWebView *web_view, WebKitLoadEvent event G_GNUC_UNUSED,
                            AppWidgets *app) {
    gtk_widget_set_sensitive(app->back_btn, webkit_web_view_can_go_back(web_view));
    gtk_widget_set_sensitive(app->forward_btn, webkit_web_view_can_go_forward(web_view));
}

static void on_progress_changed(GObject *object G_GNUC_UNUSED, GParamSpec *pspec G_GNUC_UNUSED,
                                AppWidgets *app) {
    double progress = webkit_web_view_get_estimated_load_progress(app->web_view);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->progress_bar), progress);
    gtk_widget_set_visible(app->progress_bar, progress > 0.0 && progress < 1.0);
}

static gboolean on_decide_policy(WebKitWebView *web_view, WebKitPolicyDecision *decision,
                                 WebKitPolicyDecisionType type, AppWidgets *app) {
    if (type != WEBKIT_POLICY_DECISION_TYPE_NAVIGATION_ACTION &&
        type != WEBKIT_POLICY_DECISION_TYPE_NEW_WINDOW_ACTION)
        return FALSE;

    WebKitNavigationAction *action = webkit_navigation_policy_decision_get_navigation_action(
        WEBKIT_NAVIGATION_POLICY_DECISION(decision));
    const char *uri = webkit_uri_request_get_uri(webkit_navigation_action_get_request(action));
    gboolean internal = is_internal_uri(uri);

    if (type == WEBKIT_POLICY_DECISION_TYPE_NEW_WINDOW_ACTION) {
        if (internal)
            webkit_web_view_load_uri(web_view, uri);
        else
            open_external_uri(app->window, uri);
        webkit_policy_decision_ignore(decision);
        return TRUE;
    }

    if (!internal &&
        webkit_navigation_action_get_navigation_type(action) == WEBKIT_NAVIGATION_TYPE_LINK_CLICKED) {
        open_external_uri(app->window, uri);
        webkit_policy_decision_ignore(decision);
        return TRUE;
    }

    return FALSE;
}

static gboolean on_enter_fullscreen(WebKitWebView *web_view G_GNUC_UNUSED, AppWidgets *app) {
    gtk_widget_set_visible(app->header, FALSE);
    gtk_window_fullscreen(app->window);
    return TRUE;
}

static gboolean on_leave_fullscreen(WebKitWebView *web_view G_GNUC_UNUSED, AppWidgets *app) {
    gtk_window_unfullscreen(app->window);
    gtk_widget_set_visible(app->header, TRUE);
    return TRUE;
}

static void on_filter_saved(GObject *source, GAsyncResult *res, gpointer user_data) {
    WebKitUserContentFilterStore *store = WEBKIT_USER_CONTENT_FILTER_STORE(source);
    g_autoptr(WebKitWebView) web_view = WEBKIT_WEB_VIEW(user_data);
    g_autoptr(GError) error = NULL;

    WebKitUserContentFilter *filter =
        webkit_user_content_filter_store_save_finish(store, res, &error);

    if (filter) {
        WebKitUserContentManager *manager = webkit_web_view_get_user_content_manager(web_view);
        webkit_user_content_manager_add_filter(manager, filter);
        webkit_user_content_filter_unref(filter);
    } else {
        g_warning("Błąd filtra treści: %s", error ? error->message : "nieznany");
    }

    webkit_web_view_load_uri(web_view, HOME_URL);
}

static void on_back_clicked(GtkButton *btn G_GNUC_UNUSED, AppWidgets *app) {
    webkit_web_view_go_back(app->web_view);
}

static void on_forward_clicked(GtkButton *btn G_GNUC_UNUSED, AppWidgets *app) {
    webkit_web_view_go_forward(app->web_view);
}

static void on_reload_clicked(GtkButton *btn G_GNUC_UNUSED, AppWidgets *app) {
    webkit_web_view_reload(app->web_view);
}

static void on_home_clicked(GtkButton *btn G_GNUC_UNUSED, AppWidgets *app) {
    webkit_web_view_load_uri(app->web_view, HOME_URL);
}

static void on_open_browser_clicked(GtkButton *btn G_GNUC_UNUSED, AppWidgets *app) {
    open_external_uri(app->window, webkit_web_view_get_uri(app->web_view));
}

static GtkWidget *make_header_button(const char *icon, const char *tooltip,
                                     GCallback callback, AppWidgets *app) {
    GtkWidget *button = gtk_button_new_from_icon_name(icon);
    gtk_widget_set_tooltip_text(button, tooltip);
    g_signal_connect(button, "clicked", callback, app);
    return button;
}

static void setup_header_bar(AppWidgets *app) {
    app->header      = gtk_header_bar_new();
    app->back_btn    = make_header_button("go-previous-symbolic", "Wstecz",
                                          G_CALLBACK(on_back_clicked), app);
    app->forward_btn = make_header_button("go-next-symbolic", "Dalej",
                                          G_CALLBACK(on_forward_clicked), app);
    GtkWidget *reload_btn = make_header_button("view-refresh-symbolic", "Odśwież",
                                               G_CALLBACK(on_reload_clicked), app);
    GtkWidget *home_btn   = make_header_button("go-home-symbolic", "Strona główna",
                                               G_CALLBACK(on_home_clicked), app);
    GtkWidget *ext_btn    = make_header_button("web-browser-symbolic", "Otwórz w przeglądarce",
                                               G_CALLBACK(on_open_browser_clicked), app);

    gtk_widget_set_sensitive(app->back_btn, FALSE);
    gtk_widget_set_sensitive(app->forward_btn, FALSE);

    gtk_header_bar_pack_start(GTK_HEADER_BAR(app->header), app->back_btn);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(app->header), app->forward_btn);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(app->header), reload_btn);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(app->header), home_btn);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(app->header), ext_btn);

    gtk_window_set_titlebar(app->window, app->header);
}

static WebKitUserContentManager *create_content_manager(void) {
    WebKitUserContentManager *manager = webkit_user_content_manager_new();

    if (ADSKIP_SCRIPT_JS[0] != '\0') {
        WebKitUserScript *script = webkit_user_script_new(
            ADSKIP_SCRIPT_JS,
            WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
            WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START,
            NULL, NULL);
        webkit_user_content_manager_add_script(manager, script);
        webkit_user_script_unref(script);
    }

    return manager;
}

static WebKitNetworkSession *create_network_session(const char *data_dir, const char *cache_dir) {
    g_mkdir_with_parents(data_dir, 0700);
    g_mkdir_with_parents(cache_dir, 0700);

    WebKitNetworkSession *session = webkit_network_session_new(data_dir, cache_dir);
    WebKitCookieManager *cookies = webkit_network_session_get_cookie_manager(session);

    g_autofree char *cookie_file = g_build_filename(data_dir, "cookies.sqlite", NULL);
    webkit_cookie_manager_set_persistent_storage(cookies, cookie_file,
                                                 WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);
    return session;
}

static void start_adblock_and_load(WebKitWebView *web_view, const char *cache_dir) {
    g_autofree char *filters_dir = g_build_filename(cache_dir, "filters", NULL);
    g_autoptr(WebKitUserContentFilterStore) store = webkit_user_content_filter_store_new(filters_dir);
    g_autoptr(GBytes) rules = g_bytes_new_static(ADBLOCK_RULES_JSON, strlen(ADBLOCK_RULES_JSON));

    webkit_user_content_filter_store_save(store, FILTER_ID, rules, NULL,
                                          on_filter_saved, g_object_ref(web_view));
}

static void connect_web_view_signals(AppWidgets *app) {
    g_signal_connect(app->web_view, "notify::title", G_CALLBACK(on_title_changed), app);
    g_signal_connect(app->web_view, "notify::estimated-progress", G_CALLBACK(on_progress_changed), app);
    g_signal_connect(app->web_view, "load-changed", G_CALLBACK(on_load_changed), app);
    g_signal_connect(app->web_view, "decide-policy", G_CALLBACK(on_decide_policy), app);
    g_signal_connect(app->web_view, "enter-fullscreen", G_CALLBACK(on_enter_fullscreen), app);
    g_signal_connect(app->web_view, "leave-fullscreen", G_CALLBACK(on_leave_fullscreen), app);
}

static GtkWidget *create_web_view(const char *data_dir, const char *cache_dir) {
    g_autoptr(WebKitNetworkSession) session = create_network_session(data_dir, cache_dir);
    g_autoptr(WebKitUserContentManager) content_manager = create_content_manager();
    g_autoptr(WebKitSettings) settings = webkit_settings_new_with_settings(
        "user-agent", USER_AGENT,
        "enable-fullscreen", TRUE,
        "media-playback-requires-user-gesture", FALSE,
        NULL);

    GtkWidget *web_view = g_object_new(WEBKIT_TYPE_WEB_VIEW,
                                       "network-session", session,
                                       "user-content-manager", content_manager,
                                       "settings", settings,
                                       NULL);

    gtk_widget_set_vexpand(web_view, TRUE);
    gtk_widget_set_hexpand(web_view, TRUE);
    return web_view;
}

static void activate(GtkApplication *gtk_app, gpointer user_data G_GNUC_UNUSED) {
    GtkWindow *existing = gtk_application_get_active_window(gtk_app);
    if (existing) {
        gtk_window_present(existing);
        return;
    }

    GtkWidget *window = gtk_application_window_new(gtk_app);
    gtk_window_set_title(GTK_WINDOW(window), APP_NAME);
    gtk_window_set_default_size(GTK_WINDOW(window), 1100, 720);

    AppWidgets *app = g_new0(AppWidgets, 1);
    app->window = GTK_WINDOW(window);
    g_object_set_data_full(G_OBJECT(window), APP_DATA_KEY, app, g_free);

    setup_header_bar(app);

    g_autofree char *data_dir  = g_build_filename(g_get_user_data_dir(), "ytnoads", NULL);
    g_autofree char *cache_dir = g_build_filename(g_get_user_cache_dir(), "ytnoads", NULL);

    GtkWidget *web_widget = create_web_view(data_dir, cache_dir);
    app->web_view = WEBKIT_WEB_VIEW(web_widget);

    app->progress_bar = gtk_progress_bar_new();
    gtk_widget_add_css_class(app->progress_bar, "osd");
    gtk_widget_set_valign(app->progress_bar, GTK_ALIGN_START);
    gtk_widget_set_visible(app->progress_bar, FALSE);

    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), web_widget);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), app->progress_bar);
    gtk_window_set_child(app->window, overlay);

    connect_web_view_signals(app);
    start_adblock_and_load(app->web_view, cache_dir);
    gtk_window_present(app->window);
}

int main(int argc, char **argv) {
    g_autoptr(GtkApplication) app = gtk_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
