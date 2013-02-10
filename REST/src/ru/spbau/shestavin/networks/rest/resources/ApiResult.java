package ru.spbau.shestavin.networks.rest.resources;

import java.util.Map;

public class ApiResult {
	private String disclaimer;
	private String license;
	private Integer timestamp;
	private String base;
	private Map<String, Double> rates;
	
	/**
	 * 
	 */
	public ApiResult() {
	}
	
	/**
	 * @return the disclaimer
	 */
	public String getDisclaimer() {
		return disclaimer;
	}
	/**
	 * @return the license
	 */
	public String getLicense() {
		return license;
	}
	/**
	 * @return the timestamp
	 */
	public Integer getTimestamp() {
		return timestamp;
	}
	/**
	 * @return the base
	 */
	public String getBase() {
		return base;
	}
	/**
	 * @return the rates
	 */
	public Map<String, Double> getRates() {
		return rates;
	}
}