/* Language switcher for the Iroha 1 documentation.
 *
 * Each language is a complete Sphinx build deployed as a sibling
 * directory next to the English site:
 *   /iroha1-cpp/        -> English (default)
 *   /iroha1-cpp/pl/     -> Polski
 *   /iroha1-cpp/tr/     -> Türkçe
 *
 * Keep BASE in sync with html_baseurl in conf.py and the list of
 * languages with the directories produced by docs/i18n/build*.sh.
 */
(function () {
  'use strict';

  var BASE = 'https://baziorek.github.io/iroha1-cpp/';
  var LANGS = {
    en: 'English',
    pl: 'Polski',
    tr: 'Türkçe'
    // ru: 'Русский'  -- enable together with docs/i18n/ru/
  };

  var basePath = '/iroha1-cpp/';
  try {
    basePath = new URL(BASE).pathname;
  } catch (e) { /* keep default */ }

  // Path of the current page relative to its own language root,
  // e.g. 'getting_started/index.html' or '' for the landing page.
  var pathname = location.pathname;
  var rel;
  if (pathname.indexOf(basePath) === 0) {
    rel = pathname.slice(basePath.length);
  } else {
    // Served from a different location (local preview, mirror):
    // assume the path is already relative to the site root.
    rel = pathname.replace(/^\/+/, '');
  }

  // Detect the language segment ('pl/', 'tr/', ...) if present;
  // English pages have no prefix.
  var current = 'en';
  for (var code in LANGS) {
    if (code !== 'en' && rel.indexOf(code + '/') === 0) {
      current = code;
      rel = rel.slice(code.length + 1);
      break;
    }
  }

  var names = Object.keys(LANGS);
  if (names.length < 2) return;

  var select = document.createElement('select');
  select.setAttribute('aria-label', 'Documentation language');
  select.style.cssText =
    'position:fixed;top:8px;right:8px;z-index:1000;' +
    'font:13px sans-serif;padding:4px 6px;opacity:0.85;';

  names.forEach(function (code) {
    var opt = document.createElement('option');
    opt.value = code;
    opt.textContent = LANGS[code];
    if (code === current) opt.selected = true;
    select.appendChild(opt);
  });

  select.addEventListener('change', function () {
    var code = select.value;
    var prefix = code === 'en' ? '' : code + '/';
    var target = BASE + prefix + rel;
    // keep query/hash (e.g. #anchor) across languages when possible
    location.href = target + location.hash;
  });

  function inject() {
    document.body.appendChild(select);
  }
  if (document.body) {
    inject();
  } else {
    document.addEventListener('DOMContentLoaded', inject);
  }
})();
