// Copyright 2015 Charles D. Aylward
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// A (possibly updated) copy of of this software is available at
// https://github.com/cdaylward/libappc

#pragma once

#include "appc/schema/ac_kind.h"
#include "appc/schema/ac_version.h"
#include "appc/schema/annotations.h"
#include "appc/schema/app_refs.h"
#include "appc/schema/isolators.h"
#include "appc/schema/uuid.h"
#include "appc/schema/volumes.h"
#include "appc/schema/try_json.h"


namespace appc {
namespace schema {


struct ContainerRuntimeManifest {
  const AcVersion ac_version;
  const AcKind ac_kind;
  const UUID uuid;
  const AppRefs app_refs;
  const Option<Volumes> volumes;
  const Option<Isolators> isolators;
  const Option<Annotations> annotations;

  explicit ContainerRuntimeManifest(const AcVersion& ac_version,
                                    const AcKind& ac_kind,
                                    const UUID& uuid,
                                    const AppRefs& app_refs,
                                    const Option<Volumes>& volumes = None<Volumes>(),
                                    const Option<Isolators>& isolators = None<Isolators>(),
                                    const Option<Annotations>& annotations = None<Annotations>())
    : ac_version(ac_version),
      ac_kind(ac_kind),
      uuid(uuid),
      app_refs(app_refs),
      volumes(volumes),
      isolators(isolators),
      annotations(annotations) {}

  static Try<ContainerRuntimeManifest> from_json(const Json& json) {
    const auto ac_version = try_from_json<AcVersion>(json, "acVersion");
    const auto ac_kind = try_from_json<AcKind>(json, "acKind");
    const auto uuid = try_from_json<UUID>(json, "uuid");
    const auto app_refs = try_from_json<AppRefs>(json, "apps");

    const auto volumes = try_option_from_json<Volumes>(json, "volumes");
    const auto isolators = try_option_from_json<Isolators>(json, "isolators");
    const auto annotations = try_option_from_json<Annotations>(json, "annotations");

    if (!all_results(ac_version, ac_kind, uuid, app_refs, volumes, isolators, annotations)) {
      return collect_failure_reasons<ContainerRuntimeManifest>(
        ac_version, ac_kind, uuid, app_refs, volumes, isolators, annotations);
    }

    return Result(ContainerRuntimeManifest(
        from_result(ac_version),
        from_result(ac_kind),
        from_result(uuid),
        from_result(app_refs),
        from_result(volumes),
        from_result(isolators),
        from_result(annotations)));
  }

  static Json to_json(const ContainerRuntimeManifest& crm) {
    Json json{};
    json["acVersion"] = AcVersion::to_json(crm.ac_version);
    json["acKind"] = AcKind::to_json(crm.ac_kind);
    json["uuid"] = UUID::to_json(crm.uuid);
    json["apps"] = AppRefs::to_json(crm.app_refs);
    if (crm.volumes) {
      json["volumes"] = Volumes::to_json(from_some(crm.volumes));
    }
    if (crm.isolators) {
      json["isolators"] = Isolators::to_json(from_some(crm.isolators));
    }
    if (crm.annotations) {
      json["annotations"] = Annotations::to_json(from_some(crm.annotations));
    }
    return json;
  }

  Status validate() const {
    return collect_status({
      ac_version.validate(),
      ac_kind.validate(),
      uuid.validate(),
      app_refs.validate(),
      validate_if_some(volumes),
      validate_if_some(isolators),
      validate_if_some(annotations),
    });
  }
};


} // namespace schema
} // namespace appc
