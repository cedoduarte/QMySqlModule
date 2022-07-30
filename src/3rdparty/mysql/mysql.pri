# Copyright (c) 2000, 2021, Oracle and/or its affiliates.
# Copyright (c) 2022, Carlos Enrique Duarte Ortiz (carlosduarte.1@hotmail.com)

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.

# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have included with MySQL.

# Without limiting anything contained in the foregoing, this file,
# which is part of C Driver for MySQL (Connector/C), is also subject to the
# Universal FOSS Exception, version 1.0, a copy of which can be found at
# http://oss.oracle.com/licenses/universal-foss-exception.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License, version 2.0, for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

INCLUDEPATH += "$$PWD/include"
INCLUDEPATH += "$$PWD/include/mysql"

LIBS += -L"$$PWD/lib"

LIBS += -lfido2
LIBS += -lharness-library
LIBS += -lcrypto-1_1-x64
LIBS += -lmecab
LIBS += -lprotobuf-debug
LIBS += -lprotobuf-lite-debug
LIBS += -lprotobuf-lite
LIBS += -lprotobuf
LIBS += -lssl-1_1-x64
LIBS += -lmysqlharness_stdx
LIBS += -lmysqlharness_tls
LIBS += -lmysqlrouter_http
LIBS += -lmysqlrouter_http_auth_backend
LIBS += -lmysqlrouter_http_auth_realm
LIBS += -lmysqlrouter_io_component
LIBS += -lhttp_auth_backend
LIBS += -lhttp_auth_realm
LIBS += -lhttp_server
LIBS += -lio
LIBS += -lkeepalive
LIBS += -lmysql
LIBS += -lprotobuf-lite
LIBS += -lmetadata_cache
LIBS += -ladt_null
LIBS += -lauthentication_fido_client
LIBS += -lauthentication_kerberos_client
LIBS += -lauthentication_oci_client
LIBS += -lcomponent_audit_api_message_emit
LIBS += -lcomponent_keyring_file
LIBS += -lcomponent_log_filter_dragnet
LIBS += -lcomponent_log_sink_json
LIBS += -lcomponent_log_sink_syseventlog
LIBS += -lcomponent_mysqlbackup
LIBS += -lcomponent_query_attributes
LIBS += -lcomponent_reference_cache
LIBS += -lcomponent_validate_password
LIBS += -lconnection_control
LIBS += -lddl_rewriter
LIBS += -lgroup_replication
LIBS += -lha_example
LIBS += -lha_mock
LIBS += -lkeyring_file
LIBS += -lkeyring_udf
LIBS += -lpluginmecab
LIBS += -llocking_service
LIBS += -lmypluglib
LIBS += -lmysql_clone
LIBS += -lmysql_no_login
LIBS += -lrewrite_example
LIBS += -lrewriter
LIBS += -lsemisync_master
LIBS += -lsemisync_replica
LIBS += -lsemisync_slave
LIBS += -lsemisync_source
LIBS += -lvalidate_password
LIBS += -lversion_token
LIBS += -lrest_api
LIBS += -lrest_metadata_cache
LIBS += -lrest_router
LIBS += -lrest_routing
LIBS += -lrouter_openssl
LIBS += -lrouter_protobuf
LIBS += -lrouting
