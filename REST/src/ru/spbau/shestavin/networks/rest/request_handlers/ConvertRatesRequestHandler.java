package ru.spbau.shestavin.networks.rest.request_handlers;

import java.io.IOException;
import java.util.Map;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

/**
 * @author DimaTWL 
 * Handling all requests on "rest/convertrates/{baseCurrencyName}/{baseCurrencyAmount}/{destinationCurrencyName}" 
 * URL: rest/convertrates/{baseCurrencyName}/{baseCurrencyAmount}/{destinationCurrencyName}
 * Method: GET 
 * Required response: JSON or HTML
 */
@Path("/convertrates/{baseCurrencyName}/{baseCurrencyAmount}/{destinationCurrencyName}")
public class ConvertRatesRequestHandler extends BaseRequestHandler {
	/**
	 * This method used to handle GET request on "rest/convertrates/{baseCurrencyName}/{baseCurrencyAmount}/{destinationCurrencyName}" to produce JSON
	 * 
	 * @return the JSON witch will be sent to client
	 */
	@GET
	@Produces(MediaType.APPLICATION_JSON)
	public String getJSON(
			@PathParam ("baseCurrencyName") String baseCurrencyName,
			@PathParam ("baseCurrencyAmount") String baseCurrencyAmount,
			@PathParam ("destinationCurrencyName") String destinationCurrencyName) throws IOException {
		return GSON.toJson(convert(baseCurrencyName, baseCurrencyAmount, destinationCurrencyName));
	}
	
	/**
	 * This method used to handle GET request on "rest/convertrates/{baseCurrencyName}/{baseCurrencyAmount}/{destinationCurrencyName}" to produce HTML
	 * 
	 * @return the HTML witch will be sent to client
	 */
	@GET
	@Produces(MediaType.TEXT_HTML)
	public String getHTML(
			@PathParam ("baseCurrencyName") String baseCurrencyName,
			@PathParam ("baseCurrencyAmount") String baseCurrencyAmount,
			@PathParam ("destinationCurrencyName") String destinationCurrencyName) throws IOException {
		
		Double result = convert(baseCurrencyName, baseCurrencyAmount, destinationCurrencyName);
		if (null == result) {
			return "NULL";
		} else {
			return result.toString();
		}
	}
	
}
