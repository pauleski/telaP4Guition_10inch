#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  WifiManager + CustomKeyboard — ESPHome include
//  Teclado QWERTY customizado, NVS, scan com polling
//  Autor: 2º SGT PAULESKI / DA-DCT
// ─────────────────────────────────────────────────────────────────────────────
#include "esphome/components/wifi/wifi_component.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "lvgl.h"
#include <vector>
#include <string>
#include <cstring>

#define WM_NVS_NS   "cer_wifi"
#define WM_NVS_SSID "ssid"
#define WM_NVS_PASS "pass"

// ═════════════════════════════════════════════════════════════════════════════
//  CustomKeyboard
// ═════════════════════════════════════════════════════════════════════════════
class CustomKeyboard {
 public:
  static CustomKeyboard *instance;

  lv_obj_t *container = nullptr;
  lv_obj_t *textarea  = nullptr;
  bool shift_on  = false;
  bool num_mode  = false;
  bool caps_lock = false;

  // Layouts QWERTY
  static constexpr const char *ROW0_L = "qwertyuiop";
  static constexpr const char *SYM_SHIFT = "SFT";
  static constexpr const char *SYM_DEL   = "DEL";
  static constexpr const char *SYM_CAPS  = "CAP";
  static constexpr const char *ROW1_L = "asdfghjkl";
  static constexpr const char *ROW2_L = "zxcvbnm";
  static constexpr const char *ROW0_U = "QWERTYUIOP";
  static constexpr const char *ROW1_U = "ASDFGHJKL";
  static constexpr const char *ROW2_U = "ZXCVBNM";

  // Layout numérico
  static constexpr const char *ROW0_N = "1234567890";
  static constexpr const char *ROW1_N = "@#$%&*()-_";
  static constexpr const char *ROW2_N = "!+=[]{}:;/";

  // Cores
  static constexpr uint32_t C_BG      = 0x161B22;
  static constexpr uint32_t C_KEY     = 0x21262D;
  static constexpr uint32_t C_KEY_PR  = 0x9966CB;
  static constexpr uint32_t C_SPEC    = 0x30363D;
  static constexpr uint32_t C_SPEC_PR = 0x73309C;
  static constexpr uint32_t C_ENTER   = 0x3FB950;
  static constexpr uint32_t C_TXT     = 0xE6EDF3;
  static constexpr uint32_t C_TXT_DIM = 0x8B949E;

  // setup: usa container YAML existente (posição já definida no YAML)
  void setup(lv_obj_t *cont, lv_obj_t *ta) {
    instance  = this;
    textarea  = ta;
    container = cont;
    lv_obj_set_style_bg_color(container, lv_color_hex(C_BG), 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, 6, 0);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    build_keys();
  }

  void build_keys() {
    lv_obj_clean(container);
    lv_coord_t w = 652;  // 676 - 12*2 (padding)
    lv_coord_t h = 432;  // 456 - 12*2
    lv_coord_t row_h = h / 4;
    lv_coord_t gap   = 4;

    const char *r0 = num_mode ? ROW0_N : (shift_on || caps_lock ? ROW0_U : ROW0_L);
    const char *r1 = num_mode ? ROW1_N : (shift_on || caps_lock ? ROW1_U : ROW1_L);
    const char *r2 = num_mode ? ROW2_N : (shift_on || caps_lock ? ROW2_U : ROW2_L);

    // ── Linha 0: 10 teclas ────────────────────────────────────────────────
    int n0 = strlen(r0);
    lv_coord_t kw0 = (w - gap * (n0 - 1)) / n0;
    for (int i = 0; i < n0; i++) {
      char lbl[3] = { r0[i], 0, 0 };
      make_key(container, lbl, i * (kw0 + gap), 0, kw0, row_h - gap, false);
    }

    // ── Linha 1 ───────────────────────────────────────────────────────────
    int n1 = strlen(r1);
    lv_coord_t kw1  = (w - gap * (n1)) / n1;
    lv_coord_t off1 = (w - kw1 * n1 - gap * (n1 - 1)) / 2;
    for (int i = 0; i < n1; i++) {
      char lbl[3] = { r1[i], 0, 0 };
      make_key(container, lbl, off1 + i * (kw1 + gap), row_h, kw1, row_h - gap, false);
    }

    // ── Linha 2: SHIFT + 7 teclas + BKSP ─────────────────────────────────
    int n2    = strlen(r2);
    lv_coord_t shift_w = 80;
    lv_coord_t bksp_w  = 80;
    lv_coord_t rem     = w - shift_w - bksp_w - gap * (n2 + 1);
    lv_coord_t kw2     = rem / n2;
    lv_coord_t y2      = row_h * 2;

    // Botão SHIFT / CAPS
    const char *shift_lbl = caps_lock ? "CAPS" : (shift_on ? SYM_SHIFT : SYM_SHIFT);
    lv_obj_t *btn_shift = make_key(container, shift_lbl, 0, y2, shift_w, row_h - gap, true);
    lv_obj_set_style_bg_color(btn_shift,
      lv_color_hex(caps_lock ? 0x9966CB : C_SPEC), 0);

    for (int i = 0; i < n2; i++) {
      char lbl[3] = { r2[i], 0, 0 };
      make_key(container, lbl,
        shift_w + gap + i * (kw2 + gap), y2, kw2, row_h - gap, false);
    }

    // Backspace
    lv_obj_t *btn_bk = make_key(container, SYM_DEL,
      shift_w + gap + n2 * (kw2 + gap), y2, bksp_w, row_h - gap, true);
    lv_obj_set_style_bg_color(btn_bk, lv_color_hex(0xF78166), 0);

    // ── Linha 3: 123/ABC + SPACE + ENTER ─────────────────────────────────
    lv_coord_t y3    = row_h * 3;
    lv_coord_t mode_w  = 100;
    lv_coord_t enter_w = 120;
    lv_coord_t sp_w    = w - mode_w - enter_w - gap * 2;

    const char *mode_lbl = num_mode ? "ABC" : "123";
    lv_obj_t *btn_mode = make_key(container, mode_lbl, 0, y3, mode_w, row_h - gap, true);
    lv_obj_set_style_bg_color(btn_mode, lv_color_hex(C_SPEC), 0);

    lv_obj_t *btn_sp = make_key(container, " ", mode_w + gap, y3, sp_w, row_h - gap, false);
    lv_obj_set_style_bg_color(btn_sp, lv_color_hex(C_KEY), 0);

    lv_obj_t *btn_ok = make_key(container, "OK",
      mode_w + gap + sp_w + gap, y3, enter_w, row_h - gap, true);
    lv_obj_set_style_bg_color(btn_ok, lv_color_hex(C_ENTER), 0);
  }

  lv_obj_t *make_key(lv_obj_t *parent, const char *lbl,
                      lv_coord_t x, lv_coord_t y,
                      lv_coord_t w, lv_coord_t h, bool special) {
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_style_bg_color(btn, lv_color_hex(special ? C_SPEC : C_KEY), 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(C_KEY_PR), LV_STATE_PRESSED);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x30363D), 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_radius(btn, 6, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, lbl);
    lv_obj_set_style_text_color(label, lv_color_hex(C_TXT), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // Armazena label como user_data para identificação
    lv_obj_set_user_data(btn, (void *)lv_label_get_text(label));
    lv_obj_add_event_cb(btn, key_event_cb, LV_EVENT_CLICKED, nullptr);
    return btn;
  }

  static void key_event_cb(lv_event_t *e) {
    if (!instance || !instance->textarea) return;
    lv_obj_t *btn   = static_cast<lv_obj_t *>(lv_event_get_target(e));
    lv_obj_t *child = lv_obj_get_child(btn, 0);
    if (!child) return;
    const char *lbl = lv_label_get_text(child);
    instance->handle_key(lbl);
  }

  void handle_key(const char *lbl) {
    if (!lbl) return;

    // Shift / Caps
    if (strcmp(lbl, SYM_SHIFT) == 0 || strcmp(lbl, SYM_SHIFT) == 0) {
      if (shift_on) { shift_on = false; caps_lock = true; }
      else if (caps_lock) { caps_lock = false; }
      else { shift_on = true; }
      build_keys(); return;
    }
    // Backspace
    if (strcmp(lbl, SYM_DEL) == 0) {
      uint16_t len = strlen(lv_textarea_get_text(textarea));
      if (len > 0) lv_textarea_delete_char(textarea);
      return;
    }
    // Modo numérico
    if (strcmp(lbl, "123") == 0) { num_mode = true;  build_keys(); return; }
    if (strcmp(lbl, "ABC") == 0) { num_mode = false; build_keys(); return; }
    // OK / Enter (fecha teclado visualmente — textarea mantém foco)
    if (strcmp(lbl, "OK") == 0) { return; }
    // Espaço
    if (strcmp(lbl, " ") == 0) {
      lv_textarea_add_char(textarea, ' ');
      return;
    }
    // Tecla normal
    lv_textarea_add_text(textarea, lbl);
    // Desativa shift após tecla (não caps_lock)
    if (shift_on) { shift_on = false; build_keys(); }
  }
};
CustomKeyboard *CustomKeyboard::instance = nullptr;

// ═════════════════════════════════════════════════════════════════════════════
//  WifiManager
// ═════════════════════════════════════════════════════════════════════════════
class WifiManager {
 public:
  static WifiManager *instance;

  struct ApInfo { std::string ssid; int8_t rssi; };
  std::vector<ApInfo> ap_list;
  std::string selected_ssid;
  bool scanning  = false;
  bool scan_done = false;
  lv_timer_t *poll_timer   = nullptr;
  lv_timer_t *connect_timer = nullptr;
  int connect_tries = 0;

  lv_obj_t *lv_scroll  = nullptr;
  lv_obj_t *lv_status  = nullptr;
  lv_obj_t *lv_sel_lbl = nullptr;
  lv_obj_t *lv_pass_ta = nullptr;

  CustomKeyboard kb;

  static WifiManager *init() {
    if (!instance) {
      instance = new WifiManager();
      esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE,
        [](void *, esp_event_base_t, int32_t, void *) {
          if (WifiManager::instance)
            WifiManager::instance->scan_done = true;
        }, nullptr);
    }
    return instance;
  }

  // ── Scan ───────────────────────────────────────────────────────────────────
  void start_scan() {
    if (scanning) return;
    scanning  = true;
    scan_done = false;
    ap_list.clear();
    selected_ssid.clear();

    wifi_scan_config_t cfg = {};
    cfg.show_hidden = false;
    cfg.scan_type   = WIFI_SCAN_TYPE_ACTIVE;
    esp_err_t err = esp_wifi_scan_start(&cfg, false);
    if (err != ESP_OK) {
      char msg[48];
      snprintf(msg, sizeof(msg), "Erro scan: 0x%X", (unsigned)err);
      set_status(msg);
      scanning = false;
      return;
    }
    set_status("Escaneando... aguarde");

    if (poll_timer) { lv_timer_del(poll_timer); poll_timer = nullptr; }
    poll_timer = lv_timer_create([](lv_timer_t *t) {
      WifiManager *wm = WifiManager::instance;
      if (!wm || !wm->scanning) { lv_timer_del(t); return; }
      uint16_t n = 0;
      esp_wifi_scan_get_ap_num(&n);
      static int ticks = 0; ticks++;
      if (!wm->scan_done && n == 0 && ticks < 16) return;
      ticks = 0;
      lv_timer_del(t);
      wm->poll_timer = nullptr;
      wm->collect_results();
    }, 500, nullptr);
  }

  void collect_results() {
    scanning = scan_done = false;
    uint16_t n = 0;
    esp_wifi_scan_get_ap_num(&n);
    if (n == 0) { set_status("Nenhuma rede encontrada. Tente novamente."); return; }
    std::vector<wifi_ap_record_t> records(n);
    esp_wifi_scan_get_ap_records(&n, records.data());
    for (uint16_t i = 0; i < n; i++) {
      std::string ssid(reinterpret_cast<char *>(records[i].ssid));
      if (!ssid.empty()) ap_list.push_back({ssid, records[i].rssi});
    }
    populate_scroll();
    char msg[40];
    snprintf(msg, sizeof(msg), "%d redes encontradas", (int)ap_list.size());
    set_status(msg);
  }

  // ── Scroll list ────────────────────────────────────────────────────────────
  void populate_scroll() {
    if (!lv_scroll) return;
    lv_obj_clean(lv_scroll);
    for (size_t i = 0; i < ap_list.size(); i++) {
      lv_obj_t *row = lv_button_create(lv_scroll);
      lv_obj_set_size(row, 556, 52);
      lv_obj_set_pos(row, 0, static_cast<lv_coord_t>(i * 58));
      lv_obj_set_style_bg_color(row, lv_color_hex(0x21262D), 0);
      lv_obj_set_style_bg_color(row, lv_color_hex(0x9966CB), LV_STATE_PRESSED);
      lv_obj_set_style_border_color(row, lv_color_hex(0x30363D), 0);
      lv_obj_set_style_border_width(row, 1, 0);
      lv_obj_set_style_radius(row, 6, 0);
      char buf[80];
      snprintf(buf, sizeof(buf), "  %-26s  %d dBm",
               ap_list[i].ssid.c_str(), (int)ap_list[i].rssi);
      lv_obj_t *lbl = lv_label_create(row);
      lv_label_set_text(lbl, buf);
      lv_obj_set_style_text_color(lbl, lv_color_hex(0xE6EDF3), 0);
      lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 8, 0);
      lv_obj_set_user_data(row, reinterpret_cast<void *>(static_cast<uintptr_t>(i)));
      lv_obj_add_event_cb(row, on_row_click, LV_EVENT_CLICKED, nullptr);
    }
    lv_obj_set_height(lv_scroll,
      static_cast<lv_coord_t>(ap_list.size() * 58 + 8));
  }

  static void on_row_click(lv_event_t *e) {
    if (!instance) return;
    lv_obj_t *target = static_cast<lv_obj_t *>(lv_event_get_target(e));
    uintptr_t idx = static_cast<uintptr_t>(
      reinterpret_cast<uintptr_t>(lv_obj_get_user_data(target)));
    if (idx >= instance->ap_list.size()) return;
    instance->selected_ssid = instance->ap_list[idx].ssid;
    if (instance->lv_sel_lbl)
      lv_label_set_text(instance->lv_sel_lbl, instance->selected_ssid.c_str());
    instance->set_status("Rede selecionada. Digite a senha.");
    if (instance->lv_pass_ta)
      lv_textarea_set_text(instance->lv_pass_ta, "");
  }

  // ── Connect ────────────────────────────────────────────────────────────────
  void connect() {
    if (selected_ssid.empty()) { set_status("Selecione uma rede primeiro!"); return; }
    const char *pass = lv_pass_ta ? lv_textarea_get_text(lv_pass_ta) : "";
    save_credentials(selected_ssid.c_str(), pass);
    set_status("Conectando...");

    // Usa ESPHome WiFiAP para não conflitar com o WiFi component
    esphome::wifi::WiFiAP ap;
    ap.set_ssid(selected_ssid);
    ap.set_password(std::string(pass));
    esphome::wifi::global_wifi_component->set_sta(ap);

    // Força reconexão via esp_wifi após setar a config no componente
    esp_wifi_disconnect();
    esp_wifi_connect();

    // Monitora conexão
    connect_tries = 0;
    if (connect_timer) { lv_timer_del(connect_timer); }
    connect_timer = lv_timer_create([](lv_timer_t *tmr) {
      if (!WifiManager::instance) { lv_timer_del(tmr); return; }
      auto *wm = WifiManager::instance;
      wifi_ap_record_t ap_info;
      if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        wm->set_status("Conectado!");
        wm->connect_timer = nullptr;
        lv_timer_del(tmr);
      } else {
        wm->connect_tries++;
        char msg[48];
        snprintf(msg, sizeof(msg), "Aguardando... (%ds)", wm->connect_tries);
        wm->set_status(msg);
        if (wm->connect_tries > 24) {
          wm->set_status("Falha. Verifique a senha e tente novamente.");
          wm->connect_timer = nullptr;
          lv_timer_del(tmr);
        }
      }
    }, 500, nullptr);
  }

  static void save_credentials(const char *ssid, const char *pass) {
    nvs_handle_t h;
    esp_err_t err = nvs_open(WM_NVS_NS, NVS_READWRITE, &h);
    if (err != ESP_OK) {
      ESP_LOGE("WifiMgr", "NVS open failed: 0x%X", err);
      return;
    }
    nvs_set_str(h, WM_NVS_SSID, ssid);
    nvs_set_str(h, WM_NVS_PASS, pass);
    esp_err_t cerr = nvs_commit(h);
    nvs_close(h);
    ESP_LOGI("WifiMgr", "Saved SSID='%s' err=%d", ssid, cerr);
    // Copia para RAM imediatamente para apply funcionar no proximo reboot
    strncpy(nvs_ssid, ssid, sizeof(nvs_ssid)-1);
    strncpy(nvs_pass, pass, sizeof(nvs_pass)-1);
  }

  // Chamado em priority 600 — apenas lê e armazena na RAM
  static char nvs_ssid[64];
  static char nvs_pass[64];

  static bool load_credentials_to_ram() {
    nvs_ssid[0] = nvs_pass[0] = 0;
    nvs_handle_t h;
    if (nvs_open(WM_NVS_NS, NVS_READONLY, &h) != ESP_OK) return false;
    size_t sl = sizeof(nvs_ssid), pl = sizeof(nvs_pass);
    bool ok = (nvs_get_str(h, WM_NVS_SSID, nvs_ssid, &sl) == ESP_OK) &&
              (nvs_get_str(h, WM_NVS_PASS, nvs_pass, &pl) == ESP_OK) &&
              (strlen(nvs_ssid) > 0);
    nvs_close(h);
    return ok;
  }

  // Chamado em priority -100 — WiFi já inicializado, aplica via esp_wifi_set_config
  static bool apply_credentials_from_ram() {
    if (strlen(nvs_ssid) == 0) return false;
    wifi_config_t cfg = {};
    strncpy(reinterpret_cast<char *>(cfg.sta.ssid),
            nvs_ssid, sizeof(cfg.sta.ssid) - 1);
    strncpy(reinterpret_cast<char *>(cfg.sta.password),
            nvs_pass, sizeof(cfg.sta.password) - 1);
    cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    esp_wifi_disconnect();
    esp_wifi_set_config(WIFI_IF_STA, &cfg);
    esp_wifi_connect();
    return true;
  }

  // Mantém compatibilidade — alias
  static bool load_and_apply_credentials() {
    return load_credentials_to_ram();
  }

  void set_status(const char *msg) {
    if (lv_status) lv_label_set_text(lv_status, msg);
  }
};
WifiManager *WifiManager::instance = nullptr;
char WifiManager::nvs_ssid[64] = {};
char WifiManager::nvs_pass[64] = {};
