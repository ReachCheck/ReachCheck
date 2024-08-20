/*
 * H2GIS is a library that brings spatial support to the H2 Database Engine
 * <a href="http://www.h2database.com">http://www.h2database.com</a>. H2GIS is developed by CNRS
 * <a href="http://www.cnrs.fr/">http://www.cnrs.fr/</a>.
 *
 * This code is part of the H2GIS project. H2GIS is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * Lesser General Public License as published by the Free Software Foundation;
 * version 3.0 of the License.
 *
 * H2GIS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details <http://www.gnu.org/licenses/>.
 *
 *
 * For more information, please consult: <a href="http://www.h2gis.org/">http://www.h2gis.org/</a>
 * or contact directly: info_at_h2gis.org
 */
package org.h2gis.postgis_jts;

import org.postgresql.ds.PGPoolingDataSource;

import javax.sql.ConnectionPoolDataSource;
import javax.sql.DataSource;
import javax.sql.XADataSource;
import java.sql.Driver;
import java.sql.SQLException;
import java.sql.SQLFeatureNotSupportedException;
import java.util.Properties;
import java.util.concurrent.atomic.AtomicInteger;

public class PostGISDBFactory {
    public static String JDBC_DATABASE_NAME = "databaseName";
    public static String JDBC_DATASOURCE_NAME = "dataSourceName";
    public static String JDBC_PASSWORD = "password";
    public static String JDBC_PORT_NUMBER = "portNumber";
    public static String JDBC_SERVER_NAME = "serverName";
    public static String JDBC_USER = "user";
    public static String JDBC_URL = "url";
    // org.postgresql.ds.jdbc23.AbstractJdbc23PoolingDataSource hold a static container of DataSource instance.
    // JDBC_DATASOURCE_NAME should be unique on each call of CreateDataSource with different parameters
    private static AtomicInteger dataSourceCount = new AtomicInteger(0);

    public static DataSource createDataSource(Properties properties) throws SQLException {
        if (properties == null) {
            properties = new Properties();
        }
        if(properties.getProperty(JDBC_DATASOURCE_NAME) == null) {
            properties.setProperty(JDBC_DATASOURCE_NAME, PostGISDBFactory.class.getSimpleName() + "_" +
                    dataSourceCount.getAndAdd(1));
        }
        PGPoolingDataSource dataSource = PGPoolingDataSource.getDataSource(properties.getProperty(JDBC_DATASOURCE_NAME));
        if(dataSource == null) {
            dataSource = new PGPoolingDataSource();
            // Set dataSourceName, databaseName, user, and password
            dataSource.setDataSourceName(properties.getProperty(JDBC_DATASOURCE_NAME));
            String url = properties.getProperty(JDBC_URL);
            if(url != null) {
                dataSource.setUrl(url);
            }
            dataSource.setPortNumber(Integer.valueOf(properties.getProperty(JDBC_PORT_NUMBER, Integer.toString(dataSource.getPortNumber()))));
            dataSource.setServerName(properties.getProperty(JDBC_SERVER_NAME, dataSource.getServerName()));
            dataSource.setUser(properties.getProperty(JDBC_USER, dataSource.getUser()));
            dataSource.setPassword(properties.getProperty(JDBC_PASSWORD, dataSource.getPassword()));
            dataSource.setDatabaseName(properties.getProperty(JDBC_DATABASE_NAME, dataSource.getDatabaseName()));
        }
        return new DataSourceWrapper(dataSource);
    }

    public ConnectionPoolDataSource createConnectionPoolDataSource(Properties properties) throws SQLException {
        throw new SQLFeatureNotSupportedException();
    }

    public XADataSource createXADataSource(Properties properties) throws SQLException {
        throw new SQLFeatureNotSupportedException();
    }

    public Driver createDriver(Properties properties) throws SQLException {
        throw new SQLFeatureNotSupportedException();
    }
}
