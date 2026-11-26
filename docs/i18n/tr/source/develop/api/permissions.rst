***********
İzinler
***********

Hyperledger Iroha, kullanıcılarının eylemlerini sınırlamak için
role dayalı bir erişim kontrolü sistemi kullanır. Bu sistem,
farklı erişim seviyelerine sahip kullanıcı gruplarını içeren
kullanım senaryolarını uygulamada çok yardımcı olur — varlık
transferini bile alamayan zayıf kullanıcılardan süper
kullanıcılara kadar. İzin sistemimizin güzelliği, Iroha kurulumunuz
da süper kullanıcıya sahip olmanız veya tüm olası izinleri
kullanmanız gerekmemesidir: ayrılmış ve hafif roller
oluşturabilirsiniz.

Sistemin bakımı, rollere dahil edilen rollerin ve izinlerin
ayarlanmasını içerir. Bu, sistem dağıtımının ilk adımında —
genesis block'ta — veya daha sonra Iroha ağı çalışırken
yapılabilir (bunu yapabilecek bir rol varsa :).

Bu bölüm izinleri anlamanıza ve belirli izinleri içeren rollerin
nasıl oluşturulacağına dair bir fikir vermenize yardımcı
olacaktır.
Her izin, belirli bir izni gerektiren transaction veya sorgu
yaratma yöntemini gösteren Python ile yazılmış bir örnekle
donatılmıştır. Her örnek *commons.py* modülünü kullanır; listesi
`Supplementary Sources`_ bölümünde mevcuttur.

*******************
İzin listesi
*******************

.. iroha_gen_permissions_index:: permissions/matrix.csv

********************
İzinler ayrıntılı
********************

.. iroha_gen_detailed_permissions:: permissions/matrix.csv
