# Добро пожаловать!

**Языки:** [English](../../../README.md) · [Polski](../pl/README.md) · **Русский** · [日本語](../ja/README.md) · [Türkçe](../tr/README.md)

## Об этом репозитории

Это общественный форк **Hyperledger Iroha 1** (реализация на C++).
Разработка в upstream фактически остановилась — последний публичный
коммит датируется **маем 2024 года** — однако Iroha 1 всё ещё
интенсивно используется в production рядом инициатив, включая
**Национальный банк Камбоджи** (платёжная система Bakong).

Цель этого форка — сохранить Irohу 1 пригодной для сборки, использования
и сопровождения на современных toolchain'ах — и дать проекту здоровую
вторую жизнь.

### Чем этот форк отличается от upstream

По сравнению с последним коммитом upstream (`118fced3f3`, май 2024)
этот форк добавляет:

**Модернизация сборки**

- Стандарт C++ поднят до **C++23**; сборка проверена на
  **GCC 14 / Ubuntu 24.04**
- Базовый **vcpkg** обновлён с 2021 года до ноября 2026 (protobuf,
  gRPC, Boost, OpenSSL 3.x, soci 4.x)
- Устаревшие зависимости заменены там, где достаточно стандартной
  библиотеки (`fmt` → `std::format`, `boost::format`, `boost::variant`,
  `boost::filesystem`, а также несколько неиспользуемых библиотек
  Boost удалены)
- Сборка без предупреждений (~70 warnings исправлено)

**Новые компоненты**

- **`iroha-lib/`** — автономная клиентская библиотека C++ для Iroha 1
  (транзакции, запросы, пагинация, стриминг, TLS-транспорт,
  утилиты для ключей/genesis/peer list) с 9 примерами, перенесёнными
  из `iroha-python`. См. `iroha-lib/README.md` и
  `docs/source/develop/libraries/cpp.rst`.
- **`iroha-cli/`** — классический интерактивный/неинтерактивный CLI,
  переведённый на транспорт и построение моделей `iroha-lib`,
  с поддержкой `--tls_root_cert`.
- **`iroha-python`** — Python-клиент живёт в отдельном репозитории:
  [baziorek/iroha1-python](https://github.com/baziorek/iroha1-python)
  (оригинальные исходники, обновлённые зависимости).

**Серверные исправления**

- **Keepalive** gRPC на всех серверах — исправляет обрывы соединений
  за NAT/LB, из-за которых долгоживущие стримы падали примерно раз в
  24 часа (upstream issue hyperledger/iroha#1351)
- Исправление зависания `FetchCommits`, исправление oneof в
  `GetPendingTransactions`, исправления `creator_account_id` —
  всё найдено новыми интеграционными тестами

**Релизы и CI**

- Пакеты **CPack** DEB/RPM + артефакты AppImage; **GitHub Releases**
  по тегам версий (`1.6.1-reanimation` продолжает линию версий
  upstream; версии пакетов выводятся из `git describe`)
- Модернизированные GitHub Actions: раннеры Node 24, действия
  привязаны к SHA коммитов, pip-зависимости с хэшами, общий job
  кэша зависимостей vcpkg
- **OpenSSF Scorecard** + **SECURITY.md** + Dependabot; branch
  protection на `iroha1-main`
- Стек нагрузочного тестирования переведён с `locustio` на `locust` 2.x

**Документация**

- Документация Sphinx пересобрана и опубликована на **GitHub Pages**:
  <https://baziorek.github.io/iroha1-cpp/>
- Полные переводы на **польский, русский, японский и турецкий** —
  включая этот README (см. языковые ссылки вверху)

**Удалённый мёртвый код**

- Упаковка `snap/` (не собирается на современных зависимостях)
- Скрипты `housekeeping/` (мёртвый CI Soramitsu), скрипты `utils/`
  (заменены; парсинг genesis перенесён в Python-форк)
- Устаревшие патчи vcpkg и CI-воркфлоу Iroha 2

> **Отказ от ответственности:** эти общественные форки сопровождаются
> одним человеком с помощью ИИ. Они содержат обновления сопровождения,
> системы сборки и зависимостей — не изменения функциональности Iroha —
> но **не дают гарантий для production или коммерческого использования**.
> Сама Iroha 1 — зрелый проект с реальными внедрениями (например,
> платёжная система Bakong Национального банка Камбоджи).

Остальная часть этого README описывает проект upstream и остаётся
актуальной.

## Что такое Hyperledger Iroha?

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/960/badge)](https://bestpractices.coreinfrastructure.org/projects/960)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/baziorek/iroha1-cpp/badge.svg?branch=iroha1-main)](https://scorecard.dev/viewer/?uri=github.com/baziorek/iroha1-cpp)

Iroha — это простая технология распределённых реестров (DLT), вдохновлённая
японским принципом кайдзэн — устранением избыточности (muri).
Iroha предоставляет необходимую функциональность для управления вашими
активами, информацией и идентификацией, оставаясь при этом эффективным и
заслуживающим доверия инструментом, устойчивым к сбоям
(crash fault-tolerant), для нужд вашего предприятия.

Посмотрите страницу [обзора](https://baziorek.github.io/iroha1-cpp/)
нашей документации.
[Здесь](https://www.youtube.com/channel/UCYlK9OrZo9hvNYFuf0vrwww) —
YouTube-канал, где мы публикуем встречи и обучающие видео — загляните!

<img height="300px" src="../../../docs/image_assets/Iroha_3_sm.png"
 alt="Логотип Iroha" title="Iroha" align="right" />

Iroha имеет следующие возможности:
1. Создание и управление произвольными взаимозаменяемыми активами — валютами, килограммами золота и т.д.
2. Управление учётными записями пользователей
3. Таксономия учётных записей на основе _доменов_ в системе
4. Система прав и проверка разрешений пользователей на выполнение транзакций и запросов в системе
5. Валидация бизнес-правил для транзакций и запросов в системе
6. Мультиподписанные транзакции

Iroha _устойчива к сбоям_ (Crash Fault Tolerant) и имеет собственный
алгоритм консенсуса — [YAC](https://arxiv.org/pdf/1809.00554.pdf).
В Iroha 2 он заменён новым алгоритмом —
[Sumeragi](https://iroha.tech/#sumeragi).

## Документация

Документация этого форка собирается с помощью Sphinx и публикуется на
**GitHub Pages**: <https://baziorek.github.io/iroha1-cpp/> — на английском
и на польском, русском, японском и турецком (переключатель языка
в боковой панели документации и в этом README).

#### У нас есть переводы документации!

Этот форк содержит полные переводы на **польский, русский, японский и
турецкий**, поддерживаемые в `docs/i18n/` (собственный
[репозиторий локализации](https://github.com/hyperledger/iroha-docs-l10n)
upstream охватывает более старые снимки — смело помогайте переводить
на свой язык через pull request'ы).

### Как быстро познакомиться с Iroha?

Посмотрите раздел [getting started](https://baziorek.github.io/iroha1-cpp/getting_started/index.html)
документации, чтобы начать изучение системы.
Старый sandbox Katacoda больше не работает, но
[интеграционные тесты](../../../test/) и
[примеры `iroha-lib`](../../../iroha-lib/) показывают живой узел
в действии.

### Как собрать Irohу?

Воспользуйтесь [руководством по сборке](https://baziorek.github.io/iroha1-cpp/build/index.html),
которое поможет, если вы хотите изменить код и внести вклад.

### Есть ли SDK?

Да — для этого форка: современная **клиентская библиотека C++** в
[`iroha-lib/`](../../../iroha-lib/) (этот репозиторий) и
поддерживаемый **Python**-клиент по адресу
[baziorek/iroha1-python](https://github.com/baziorek/iroha1-python).
В upstream также есть SDK на
[Java](https://github.com/hyperledger/iroha-java),
[Python](https://github.com/hyperledger/iroha-python),
[JavaScript](https://github.com/hyperledger/iroha-javascript) и
[iOS](https://github.com/hyperledger/iroha-ios) (не поддерживаются,
сохранены для справки).

### Есть ли примеры приложений?

[Android point app](https://github.com/hyperledger/iroha-android/tree/master/iroha-android-sample)
и [JavaScript-кошелёк](https://github.com/soramitsu/iroha-wallet-js).

Не забудьте также о [The Borsello App](https://github.com/claudiocandio/borsello) —
это кошелёк для Android и iOS вместе с веб-приложением, внесённый
[Claudio](https://github.com/claudiocandio). Он основан на JS Wallet,
но актуален.

### Отличные инструменты для Iroha

[Iroha Explorer](https://codeberg.org/diva.exchange/iroha-explorer)
[Docker-контейнер Iroha с Postgres 10](https://hub.docker.com/r/divax/iroha)
и его [исходный код](https://codeberg.org/diva.exchange/iroha)
[Инструмент для развёртывания экземпляров Iroha](https://github.com/kuvaldini/iroha-swarm)

### Хотите помочь развивать Irohу?

Отлично!
Ознакомьтесь с [CONTRIBUTING.rst](../../../CONTRIBUTING.rst) — заявки
на issues и предложения по улучшению приветствуются через
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues)
этого репозитория.

## Нужна помощь?

* Присоединяйтесь к [Telegram-чату](https://t.me/hyperledgeriroha) или
[Hyperledger RocketChat](https://chat.hyperledger.org/channel/iroha),
где мейнтейнеры, контрибьюторы и пользователи рады помочь.
Вы также можете обсуждать свои вопросы и предложения или просто
общаться об Iroha там или в Gitter
[![Присоединяйтесь к чату на https://gitter.im/hyperledger-iroha/Lobby](https://badges.gitter.im/hyperledger-iroha/Lobby.svg)](https://gitter.im/hyperledger-iroha/Lobby)
* Отправляйте issues и предложения по улучшению через
[GitHub Issues](https://github.com/baziorek/iroha1-cpp/issues) —
по вопросам Iroha 2 upstream смотрите
[Hyperledger Jira](https://jira.hyperledger.org/secure/CreateIssue!default.jspa)
* Подпишитесь на нашу [рассылку](https://lists.hyperledger.org/g/iroha),
чтобы получать последние и самые важные новости и делиться своим
словом в сообществе Iroha

## Лицензия

Кодовая база Iroha лицензирована по Apache License,
Version 2.0 («Лицензия»); вы не можете использовать этот файл иначе,
чем в соответствии с Лицензией. Копию Лицензии можно получить по адресу
http://www.apache.org/licenses/LICENSE-2.0

Если иное не требуется применимым законодательством или не согласовано
в письменной форме, программное обеспечение распространяется по Лицензии
НА УСЛОВИЯХ «КАК ЕСТЬ», БЕЗ ГАРАНТИЙ И ОТВЕТСТВЕННОСТИ КАКОГО-ЛИБО РОДА,
явных или подразумеваемых. См. Лицензию для конкретных условий
предоставления прав и ограничений.

Файлы документации Iroha доступны по лицензии Creative Commons
Attribution 4.0 International License (CC-BY-4.0), доступной по адресу
http://creativecommons.org/licenses/by/4.0/
