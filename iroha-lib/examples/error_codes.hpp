#pragma once

#include <string>
#include <unordered_map>


/**
 * Human-readable descriptions of command error codes.
 * Port of examples/utilities/errorCodes2Hr.py from iroha-python.
 * Result codes are translated for text from documentation:
 * https://iroha.readthedocs.io/en/develop/develop/api/commands.html
 */

inline const std::unordered_map<std::string, std::unordered_map<uint32_t, std::string>>& errorCodesPerCommand()
{
    static const std::unordered_map<std::string, std::unordered_map<uint32_t, std::string>> codes{
        {"CreateDomain", {
            {0, "OK"},
            {1, "Could not create role (Internal error happened)"},
            {2, "No such permissions"},
            {3, "Domain already exists"},
            {4, "No default role found"},
        }},
        {"CreateAsset", {
            {0, "OK"},
            {1, "Could not create asset"},
            {2, "No such permissions"},
            {3, "No such domain"},
            {4, "Asset already exists"},
        }},
        {"AddAssetQuantity", {
            {0, "OK"},
            {1, "Could not add asset quantity/transfer asset"},
            {2, "No such permissions"},
            {3, "No such asset/No such source account"},
            {4, "Summation overflow (Resulting asset quantity is greater than the system can support)/No such destination account"},
            {5, "No such asset found"},
            {6, "Not enough balance"},
            {7, "Too much asset to transfer"},
        }},
        {"CreateAccount", {
            {0, "OK"},
            {1, "Could not create account"},
            {2, "No such permissions"},
            {3, "No such domain"},
            {4, "Account already exists"},
        }},
        {"TransferAsset", {
            {0, "OK"},
            {1, "Could not transfer asset (Internal error happened)"},
            {2, "No such permissions"},
            {3, "No such source account"},
            {4, "No such destination account"},
            {5, "No such asset found"},
            {6, "Not enough balance"},
            {7, "Too much asset to transfer"},
        }},
        {"GrantPermission", {
            {0, "OK"},
            {1, "Could not grant permission (Internal error happened)"},
            {2, "No such permissions"},
            {3, "No such account"},
        }},
        {"SetAccountDetail", {
            {0, "OK"},
            {1, "Could not set account detail (Internal error happened)"},
            {2, "No such permissions"},
            {3, "No such account"},
        }},
        {"CompareAndSetAccountDetail", {
            {0, "OK"},
            {1, "Could not compare and set account detail (Internal error happened)"},
            {2, "No such permissions"},
            {3, "No such account"},
            {4, "No match values"},
        }},
        {"CreateRole", {
            {0, "OK"},
            {1, "Could not create role (Internal error happened)"},
            {2, "No such permissions"},
            {3, "Role already exists"},
        }},
        {"AppendRole", {
            {0, "OK"},
            {1, "Could not append role (Internal error happened)"},
            {2, "No such permissions"},
            {3, "No such account"},
            {4, "No such role"},
        }},
        {"AddPeer", {
            {0, "OK"},
            {1, "Could not add peer (Internal error happened)"},
            {2, "No such permissions"},
        }},
        {"RemovePeer", {
            {0, "OK"},
            {1, "Could not remove peer (Internal error happened)"},
            {2, "No such permissions"},
            {3, "No such peer"},
            {4, "Network size does not allow to remove peer (After removing the peer the network would be empty)"},
        }},
        {"AddSignatory", {
            {0, "OK"},
            {1, "Could not add signatory (Internal error happened)"},
            {2, "No such permissions"},
            {3, "No such account"},
            {4, "Signatory already exists"},
        }},
        {"RemoveSignatory", {
            {0, "OK"},
            {1, "Could not remove signatory (Internal error happened)"},
            {2, "No such permissions"},
            {3, "No such account"},
            {4, "No such signatory"},
            {5, "Quorum does not allow to remove signatory (After removing the signatory account will be left with less signatories, than its quorum allows)"},
        }},
        {"SetAccountQuorum", {
            {0, "OK"},
            {1, "Could not set quorum (Internal error happened)"},
            {2, "No such permissions"},
            {3, "No such account"},
            {4, "No signatories on account (Add some signatories before setting quorum)"},
            {5, "New quorum is incorrect (New quorum size is less than account's signatories amount)"},
        }},
        {"GetAccountAssetTransactions", {
            {0, "OK"},
            {1, "Could not get account transactions (Internal error happened)"},
            {2, "No such permissions"},
            {3, "Invalid signatures"},
            {4, "Invalid pagination hash"},
            {5, "Invalid account id"},
        }},
    };
    return codes;
}


inline std::string errorCodeToHumanReadable(
        const std::string& command_name,
        uint32_t error_code)
{
    const auto& commands = errorCodesPerCommand();
    const auto it = commands.find(command_name);
    if (it == commands.end()) {
        return "";
    }
    const auto& codes = it->second;
    const auto code_it = codes.find(error_code);
    if (code_it == codes.end()) {
        return "";
    }
    return code_it->second;
}
