package ru.spbau.shestavin.networks.rest.request_handlers;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import javax.ws.rs.GET;
import javax.ws.rs.OPTIONS;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.WebApplicationException;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

/**
 * @author DimaTWL 
 * Handling all requests on "rest/historicrates/{currencyName}" 
 * URL: rest/historicrates/{currencyName} 
 * Method: GET 
 * Required response: JSON or HTML
 */
@Path("/historicrates/{currencyName}")
public class HistoricRatesRequestHandler extends BaseRequestHandler {
	/**
	 * This method used to handle GET request on "/historicrates/{currencyName}" to produce JSON
	 * 
	 * @return the JSON witch will be sent to client
	 */
	@GET
	@Produces(MediaType.APPLICATION_JSON)
	public String getJSON(@PathParam ("currencyName") String currencyName) throws IOException {
		return GSON.toJson(getLastTenDaysRates(currencyName));
	}
	
	/**
	 * This method used to handle GET request on "/historicrates/{currencyName}" to produce HTML
	 * 
	 * @return the HTML witch will be sent to client
	 */
	@GET
	@Produces(MediaType.TEXT_HTML)
	public String getHTML(@PathParam ("currencyName") String currencyName) throws IOException {
		String result = "<html>";
		result += "<head></head>";
		result += "<table>";
		for (Map.Entry<String, Double> entry : getLastTenDaysRates(currencyName).entrySet()) {
			result += "<tr><td>" + entry.getKey() + "</td><td>" + entry.getValue() + "</td></tr>";
		}
		result += "</table>";
		return result;
	}
	
	/**
	 * This method used to handle OPTIONS request on "/" to produce JSON
	 * 
	 * @return the JSON witch will be sent to client
	 */
	@OPTIONS
	@Produces(MediaType.APPLICATION_JSON)
	public Response getJSONOptions() throws IOException {
		Map<String, List<String>> result = new TreeMap<String, List<String>>();
		List<String> requests = new ArrayList<String>();
		requests.add("/exchangerates");
		requests.add("/historicrates/{currencyName}");
		requests.add("/convertrates/{baseCurrencyName}/{baseCurrencyAmount}/{destinationCurrencyName}");
		result.put("Available requests", requests);
		return Response.ok(GSON.toJson(result), MediaType.APPLICATION_JSON)
				.header("Allow", "OPTIONS,GET,HEAD")
				.build();
	}
	
}
