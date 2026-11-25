.. _node-build:

=================================
ソースからビルドする Iroha ノード
=================================

ソースからビルドした Iroha は、リリースパッケージと同じく通常の
バイナリとして ``irohad`` （ノード）と ``iroha-cli`` （コマンド
ラインクライアント）を生成します。ただしコンパイラフラグ、
パッチ、設定を完全に制御できます。

プラットフォーム別の完全なビルドガイド — 要件、vcpkg 依存関係の
ビルド、CMake オプション、トラブルシューティング — は
:ref:`build-guide` を参照してください。概要:

#. プラットフォーム（Linux/macOS または WSL 経由の Windows）の
   ツールチェーン要件を満たしてください。
#. ``./vcpkg/build_iroha_deps.sh`` で依存関係を一度ビルドします。
#. 設定してビルド:

   .. code-block:: shell

     cmake -S . -B build \
         -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg-build/scripts/buildsystems/vcpkg.cmake \
         -DCMAKE_BUILD_TYPE=Release
     cmake --build build --target irohad iroha-cli -j$(nproc)

生成されたバイナリは ``build/bin/`` に配置されます
（``irohad`` 、``iroha-cli`` ）。

ノードの起動
------------

自分でビルドした ``irohad`` は、AppImage ビルドと同じコマンドライン
フラグを受け付けます。初回起動の完全な例は :ref:`node-appimage`
を参照してください。また ``example/`` の設定例（config、genesis
block、鍵ペア）も必要です。

.. hint:: 動作するノードが必要なだけなら、
  :ref:`Docker <node-docker>` または :ref:`AppImage <node-appimage>`
  の方が速いです。ソースビルドは主に Iroha 自体を変更する開発者向け
  です。
