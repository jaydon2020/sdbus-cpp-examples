
#include "packagekit_transaction.h"

#include "../utils/logging.h"
#include "../utils/utils.h"

PackageKitTransaction::PackageKitTransaction(
    sdbus::IConnection& connection,
    const sdbus::ObjectPath& objectPath)
    : ProxyInterfaces{connection,
                      sdbus::ServiceName(PackageKitTransaction::INTERFACE_NAME),
                      objectPath} {
  registerProxy();
}

PackageKitTransaction::~PackageKitTransaction() {
  unregisterProxy();
}

void PackageKitTransaction::onPropertiesChanged(
    const sdbus::InterfaceName& interfaceName,
    const std::map<sdbus::PropertyName, sdbus::Variant>& changedProperties,
    const std::vector<sdbus::PropertyName>& invalidatedProperties) {
  Utils::print_changed_properties(interfaceName, changedProperties,
                                  invalidatedProperties);
}

void PackageKitTransaction::onCategory(const std::string& parent_id,
                                       const std::string& cat_id,
                                       const std::string& name,
                                       const std::string& summary,
                                       const std::string& icon) {
  LOG_INFO("PackageKitTransaction::onCategory");
}

void PackageKitTransaction::onDetails(
    const std::map<std::string, sdbus::Variant>& data) {
  LOG_INFO("PackageKitTransaction::onDetails");
}

void PackageKitTransaction::onErrorCode(const uint32_t& code,
                                        const std::string& details) {
  LOG_INFO("PackageKitTransaction::onErrorCode");
}

void PackageKitTransaction::onFiles(const std::string& package_id,
                                    const std::vector<std::string>& file_list) {
  LOG_INFO("PackageKitTransaction::onFiles");
}

void PackageKitTransaction::onFinished(const uint32_t& exit,
                                       const uint32_t& runtime) {
  LOG_INFO("PackageKitTransaction::onFinished");
}

void PackageKitTransaction::onPackage(const uint32_t& info,
                                      const std::string& package_id,
                                      const std::string& summary) {
  LOG_INFO("PackageKitTransaction::onPackage");
}

void PackageKitTransaction::onPackages(
    const std::vector<sdbus::Struct<uint32_t, std::string, std::string>>&
        packages) {
  LOG_INFO("PackageKitTransaction::onPackages");
}

void PackageKitTransaction::onRepoDetail(const std::string& repo_id,
                                         const std::string& description,
                                         const bool& enabled) {
  LOG_INFO("PackageKitTransaction::onRepoDetail");
}

void PackageKitTransaction::onRepoSignatureRequired(
    const std::string& package_id,
    const std::string& repository_name,
    const std::string& key_url,
    const std::string& key_userid,
    const std::string& key_id,
    const std::string& key_fingerprint,
    const std::string& key_timestamp,
    const uint32_t& type) {
  LOG_INFO("PackageKitTransaction::onRepoSignatureRequired");
}

void PackageKitTransaction::onEulaRequired(
    const std::string& eula_id,
    const std::string& package_id,
    const std::string& vendor_name,
    const std::string& license_agreement) {
  LOG_INFO("PackageKitTransaction::onEulaRequired");
}

void PackageKitTransaction::onMediaChangeRequired(
    const uint32_t& media_type,
    const std::string& media_id,
    const std::string& media_text) {
  LOG_INFO("PackageKitTransaction::onMediaChangeRequired");
}

void PackageKitTransaction::onRequireRestart(const uint32_t& type,
                                             const std::string& package_id) {
  LOG_INFO("PackageKitTransaction::onRequireRestart");
}

void PackageKitTransaction::onTransaction(const sdbus::ObjectPath& object_path,
                                          const std::string& timespec,
                                          const bool& succeeded,
                                          const uint32_t& role,
                                          const uint32_t& duration,
                                          const std::string& data,
                                          const uint32_t& uid,
                                          const std::string& cmdline) {
  LOG_INFO("PackageKitTransaction::onTransaction");
}

void PackageKitTransaction::onUpdateDetail(
    const std::string& package_id,
    const std::vector<std::string>& updates,
    const std::vector<std::string>& obsoletes,
    const std::vector<std::string>& vendor_urls,
    const std::vector<std::string>& bugzilla_urls,
    const std::vector<std::string>& cve_urls,
    const uint32_t& restart,
    const std::string& update_text,
    const std::string& changelog,
    const uint32_t& state,
    const std::string& issued,
    const std::string& updated) {
  LOG_INFO("PackageKitTransaction::onUpdateDetail");
}

void PackageKitTransaction::onUpdateDetails(
    const std::vector<sdbus::Struct<std::string,
                                    std::vector<std::string>,
                                    std::vector<std::string>,
                                    std::vector<std::string>,
                                    std::vector<std::string>,
                                    std::vector<std::string>,
                                    uint32_t,
                                    std::string,
                                    std::string,
                                    uint32_t,
                                    std::string,
                                    std::string>>& details) {
  LOG_INFO("PackageKitTransaction::onUpdateDetails");
}

void PackageKitTransaction::onDistroUpgrade(const uint32_t& type,
                                            const std::string& name,
                                            const std::string& summary) {
  LOG_INFO("PackageKitTransaction::onDistroUpgrade: {}, {}, {}", type, name,
           summary);
}

void PackageKitTransaction::onItemProgress(const std::string& id,
                                           const uint32_t& status,
                                           const uint32_t& percentage) {
  LOG_INFO("PackageKitTransaction::onItemProgress: {}, {}, {}", id, status,
           percentage);
}

void PackageKitTransaction::onDestroy() {
  LOG_INFO("PackageKitTransaction::onDestroy");
}
